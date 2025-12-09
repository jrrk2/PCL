#include "LuaScriptExporter.h"

#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QFile>
#include <QDir>
#include <QDebug>

// ============================================================================
// LuaScriptExporter Implementation
// ============================================================================

LuaScriptExporter::LuaScriptExporter(QWidget* rootWidget,
                                     const LuaExportOptions& opt)
    : m_root(rootWidget)
    , m_opt(opt)
{
    if (!m_root) {
        qWarning() << "LuaScriptExporter: null root widget";
        return;
    }
    
    // Auto-generate script name if not provided
    if (m_opt.scriptName.isEmpty()) {
        m_opt.scriptName = m_opt.processName + ".lua";
    }
    
    // Extract all parameters from widget tree
    extractParameters();
    
    // Optionally detect parameter groups
    if (m_opt.groupParameters) {
        detectGroups();
    }
}

void LuaScriptExporter::extractParameters()
{
    m_parameters.clear();
    
    if (!m_root) return;
    
    qDebug() << "\n=== LuaScriptExporter: Widget Traversal ===";
    qDebug() << "Root widget:" << m_root;
    qDebug() << "  Type:" << m_root->metaObject()->className();
    qDebug() << "  ObjectName:" << m_root->objectName();
    qDebug() << "  Size:" << m_root->size();
    
    // Traverse all child widgets
    QList<QWidget*> widgets = m_root->findChildren<QWidget*>();
    
    qDebug() << "\nFound" << widgets.size() << "child widgets";
    qDebug() << "Analyzing each widget...\n";
    
    QStringList traversalLog;
    traversalLog << "-- Widget Traversal Log:";
    traversalLog << QString("--   Root: %1 (%2)")
        .arg(m_root->objectName())
        .arg(m_root->metaObject()->className());
    traversalLog << QString("--   Children: %1").arg(widgets.size());
    traversalLog << "--";
    
    int acceptedCount = 0;
    int skippedCount = 0;
    
    for (QWidget* widget : widgets) {
        QString objectName = widget->objectName();
        QString className = QString::fromLatin1(widget->metaObject()->className());
        QString controlType = className;
        controlType.remove("Q");  // QSlider -> Slider
        
        // Log this widget
        QString logLine = QString("--   [%1] %2 \"%3\"")
            .arg(className)
            .arg(controlType)
            .arg(objectName);
        
        // Decide if we should skip this widget
        bool skip = false;
        QString skipReason;
        
        // Skip widgets without meaningful names
        if (objectName.isEmpty()) {
            skip = true;
            skipReason = "no objectName";
        }
        // Skip internal Qt widgets
        else if (objectName.startsWith("qt_")) {
            skip = true;
            skipReason = "internal Qt widget";
        }
        // Skip certain widget types (labels, buttons, containers)
        else if (qobject_cast<QLabel*>(widget)) {
            skip = true;
            skipReason = "QLabel (display only)";
        }
        else if (qobject_cast<QPushButton*>(widget)) {
            skip = true;
            skipReason = "QPushButton (action, not parameter)";
        }
        else if (qobject_cast<QGroupBox*>(widget)) {
            skip = true;
            skipReason = "QGroupBox (container, not parameter)";
        }
        else if (className == "QWidget") {
            // Generic QWidget - probably a container
            skip = true;
            skipReason = "generic container";
        }
        
        if (skip) {
            logLine += QString(" → SKIP (%1)").arg(skipReason);
            traversalLog << logLine;
            skippedCount++;
            qDebug() << "  SKIP:" << className << objectName << "-" << skipReason;
            continue;
        }
        
        // This is a parameter widget
        logLine += " → ACCEPT";
        traversalLog << logLine;
        acceptedCount++;
        
        qDebug() << "  ACCEPT:" << className << objectName;
        
        // Create parameter
        LuaParameter param;
        param.controlId = objectName;
        param.controlType = controlType;
        
        // Extract parameter name from objectName
        // Convention: parameterName_ControlType or just parameterName
        QStringList parts = objectName.split('_');
        if (!parts.isEmpty()) {
            param.name = sanitizeParameterName(parts[0]);
        } else {
            param.name = sanitizeParameterName(objectName);
        }
        
        // Determine Lua type and extract value
        param.type = determineLuaType(widget, controlType);
        param.value = extractValue(widget, controlType);
        
        qDebug() << "    → Parameter:" << param.name;
        qDebug() << "       Type:" << param.type;
        qDebug() << "       Value:" << param.value;
        
        // Add comment
        if (m_opt.includeComments) {
            param.comment = QString("from %1 \"%2\"")
                .arg(controlType)
                .arg(objectName);
        }
        
        // Detect group (optional)
        if (m_opt.groupParameters) {
            param.group = detectGroup(objectName);
        }
        
        m_parameters.append(param);
    }
    
    traversalLog << "--";
    traversalLog << QString("--   Summary: %1 accepted, %2 skipped, %3 total")
        .arg(acceptedCount)
        .arg(skippedCount)
        .arg(widgets.size());
    
    // Store traversal log for inclusion in output
    m_traversalLog = traversalLog;
    
    qDebug() << "\n=== Extraction Summary ===";
    qDebug() << "Total widgets:" << widgets.size();
    qDebug() << "Accepted as parameters:" << acceptedCount;
    qDebug() << "Skipped:" << skippedCount;
    qDebug() << "Parameters extracted:" << m_parameters.size();
    qDebug() << "========================\n";
}

QString LuaScriptExporter::extractValue(QWidget* widget, const QString& controlType)
{
    // Slider
    if (QSlider* slider = qobject_cast<QSlider*>(widget)) {
        double normalized = static_cast<double>(slider->value() - slider->minimum()) /
                          (slider->maximum() - slider->minimum());
        return QString::number(normalized, 'f', 6);
    }
    
    // CheckBox
    if (QCheckBox* checkbox = qobject_cast<QCheckBox*>(widget)) {
        return checkbox->isChecked() ? "true" : "false";
    }
    
    // RadioButton
    if (QRadioButton* radio = qobject_cast<QRadioButton*>(widget)) {
        return radio->isChecked() ? "true" : "false";
    }
    
    // LineEdit
    if (QLineEdit* edit = qobject_cast<QLineEdit*>(widget)) {
        QString text = edit->text();
        // Escape quotes
        text.replace("\"", "\\\"");
        return QString("\"%1\"").arg(text);
    }
    
    // SpinBox
    if (QSpinBox* spinbox = qobject_cast<QSpinBox*>(widget)) {
        return QString::number(spinbox->value());
    }
    
    // DoubleSpinBox
    if (QDoubleSpinBox* dspinbox = qobject_cast<QDoubleSpinBox*>(widget)) {
        return QString::number(dspinbox->value(), 'f', 6);
    }
    
    // ComboBox
    if (QComboBox* combo = qobject_cast<QComboBox*>(widget)) {
        QString text = combo->currentText();
        text.replace("\"", "\\\"");
        return QString("\"%1\"").arg(text);
    }
    
    return "nil";
}

QString LuaScriptExporter::determineLuaType(QWidget* widget, 
                                           const QString& controlType)
{
    if (qobject_cast<QCheckBox*>(widget) || qobject_cast<QRadioButton*>(widget)) {
        return "boolean";
    }
    
    if (qobject_cast<QSlider*>(widget) || 
        qobject_cast<QSpinBox*>(widget) ||
        qobject_cast<QDoubleSpinBox*>(widget)) {
        return "number";
    }
    
    if (qobject_cast<QLineEdit*>(widget) || qobject_cast<QComboBox*>(widget)) {
        return "string";
    }
    
    return "string";  // Default
}

QString LuaScriptExporter::sanitizeParameterName(const QString& name)
{
    QString result = name;
    
    // Remove common suffixes that are redundant
    QStringList suffixesToRemove = {
        "Slider", "slider",
        "Edit", "edit", 
        "LineEdit", "lineedit",
        "CheckBox", "checkbox",
        "SpinBox", "spinbox",
        "ComboBox", "combobox",
        "Control", "control"
    };
    
    for (const QString& suffix : suffixesToRemove) {
        if (result.endsWith(suffix, Qt::CaseInsensitive)) {
            result.chop(suffix.length());
            break;
        }
    }
    
    // Convert to camelCase
    if (result.contains('_')) {
        QStringList parts = result.split('_');
        result = parts[0].toLower();
        for (int i = 1; i < parts.size(); ++i) {
            if (!parts[i].isEmpty()) {
                result += parts[i][0].toUpper() + parts[i].mid(1).toLower();
            }
        }
    } else {
        if (!result.isEmpty()) {
            result[0] = result[0].toLower();
        }
    }
    
    // Remove invalid Lua identifier characters
    result.remove(QRegExp("[^a-zA-Z0-9_]"));
    
    // Ensure it doesn't start with a number
    if (!result.isEmpty() && result[0].isDigit()) {
        result = "param_" + result;
    }
    
    // If empty after all that, generate a name
    if (result.isEmpty()) {
        result = "parameter";
    }
    
    return result;
}

void LuaScriptExporter::detectGroups()
{
    // Simple heuristic: look for common prefixes
    QMap<QString, QStringList> prefixGroups;
    
    for (LuaParameter& param : m_parameters) {
        // Extract prefix before first uppercase letter after position 0
        QString prefix;
        for (int i = 1; i < param.name.length(); ++i) {
            if (param.name[i].isUpper()) {
                prefix = param.name.left(i);
                break;
            }
        }
        
        if (!prefix.isEmpty() && prefix.length() > 2) {
            param.group = prefix;
        }
    }
}

QString LuaScriptExporter::detectGroup(const QString& objectName)
{
    // Check if widget is inside a QGroupBox
    // This would require parent traversal - simplified for now
    return QString();
}

void LuaScriptExporter::writeLuaScript(QTextStream& out)
{
    writeHeader(out);
    
    if (m_opt.includeMetadata) {
        writeMetadataComment(out);
    }
    
    writeParameterTable(out);
    writeExecuteFunction(out);
    writeFooter(out);
}

void LuaScriptExporter::writeHeader(QTextStream& out)
{
    out << "-- Auto-generated Lua automation script\n";
    out << "-- Module: " << m_opt.moduleName << "\n";
    out << "-- Process: " << m_opt.processName << "\n";
    out << "-- Generated: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    out << "-- Script: " << m_opt.scriptName << "\n";
    out << "\n";
}

void LuaScriptExporter::writeMetadataComment(QTextStream& out)
{
    out << "-- Metadata:\n";
    out << "--   Parameters: " << m_parameters.size() << "\n";
    
    // Count by type
    int numBooleans = 0, numNumbers = 0, numStrings = 0;
    for (const LuaParameter& p : m_parameters) {
        if (p.type == "boolean") numBooleans++;
        else if (p.type == "number") numNumbers++;
        else if (p.type == "string") numStrings++;
    }
    
    out << "--   Booleans: " << numBooleans << "\n";
    out << "--   Numbers: " << numNumbers << "\n";
    out << "--   Strings: " << numStrings << "\n";
    out << "\n";

    // ADD TRAVERSAL LOG
    if (!m_traversalLog.isEmpty()) {
        for (const QString& line : m_traversalLog) {
            out << line << "\n";
        }
        out << "\n";
    }
}

void LuaScriptExporter::writeParameterTable(QTextStream& out)
{
    if (m_opt.groupParameters) {
        // Group by prefix or detected groups
        QMap<QString, QList<LuaParameter>> groups;
        QList<LuaParameter> ungrouped;
        
        for (const LuaParameter& param : m_parameters) {
            if (!param.group.isEmpty()) {
                groups[param.group].append(param);
            } else {
                ungrouped.append(param);
            }
        }
        
        out << "local process = {\n";
        
        // Write grouped parameters
        for (auto it = groups.begin(); it != groups.end(); ++it) {
            out << "    -- " << it.key() << " parameters\n";
            out << "    " << it.key() << " = {\n";
            
            for (const LuaParameter& param : it.value()) {
                if (m_opt.includeComments && !param.comment.isEmpty()) {
                    out << "        -- " << param.comment << "\n";
                }
                out << "        " << param.name << " = " << param.value << ",\n";
            }
            
            out << "    },\n\n";
        }
        
        // Write ungrouped parameters
        for (const LuaParameter& param : ungrouped) {
            if (m_opt.includeComments && !param.comment.isEmpty()) {
                out << "    -- " << param.comment << "\n";
            }
            out << "    " << param.name << " = " << param.value << ",\n";
        }
        
        out << "}\n\n";
    } else {
        // Flat structure
        out << "local process = {\n";
        
        for (const LuaParameter& param : m_parameters) {
            if (m_opt.includeComments && !param.comment.isEmpty()) {
                out << "    -- " << param.comment << "\n";
            }
            out << "    " << param.name << " = " << param.value << ",\n";
        }
        
        out << "}\n\n";
    }
}

void LuaScriptExporter::writeExecuteFunction(QTextStream& out)
{
    out << "-- Execute the process\n";
    out << "function execute()\n";
    out << "    print(\"Executing " << m_opt.processName << " with parameters:\")\n";
    out << "    for k, v in pairs(process) do\n";
    out << "        if type(v) == \"table\" then\n";
    out << "            print(string.format(\"  %s:\", k))\n";
    out << "            for k2, v2 in pairs(v) do\n";
    out << "                print(string.format(\"    %s = %s\", k2, tostring(v2)))\n";
    out << "            end\n";
    out << "        else\n";
    out << "            print(string.format(\"  %s = %s\", k, tostring(v)))\n";
    out << "        end\n";
    out << "    end\n";
    out << "    \n";
    out << "    -- TODO: Call actual module execution\n";
    out << "    -- " << m_opt.moduleName << "." << m_opt.processName << ".execute(process)\n";
    out << "end\n\n";
}

void LuaScriptExporter::writeFooter(QTextStream& out)
{
    out << "-- Run if this is the main script\n";
    out << "if arg and arg[0]:match(\"" << m_opt.scriptName << "\") then\n";
    out << "    execute()\n";
    out << "end\n\n";
    out << "return process\n";
}

bool LuaScriptExporter::exportToFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "LuaScriptExporter: Cannot write to" << filePath;
        return false;
    }
    
    QTextStream out(&file);
    writeLuaScript(out);
    file.close();
    
    qInfo() << "Lua script exported:" << filePath;
    return true;
}

bool LuaScriptExporter::exportWidget(QWidget* widget,
                                    const QString& moduleName,
                                    const QString& processName,
                                    const QString& outputPath)
{
    LuaExportOptions opt;
    opt.moduleName = moduleName;
    opt.processName = processName;
    
    LuaScriptExporter exporter(widget, opt);
    return exporter.exportToFile(outputPath);
}

// ============================================================================
// LuaExportHelper Implementation
// ============================================================================

bool LuaExportHelper::exportInterface(QWidget* widget,
                                     const QString& moduleName,
                                     const QString& processName,
                                     const QString& outputDir)
{
    if (!widget) {
        qWarning() << "LuaExportHelper: null widget";
        return false;
    }
    
    // Create output directory
    QDir dir(outputDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "LuaExportHelper: cannot create directory" << outputDir;
            return false;
        }
    }
    
    LuaExportOptions opt;
    opt.moduleName = moduleName;
    opt.processName = processName;
    opt.includeComments = true;
    opt.includeMetadata = true;
    opt.groupParameters = true;
    
    LuaScriptExporter exporter(widget, opt);
    
    QString outputPath = dir.filePath(processName + ".lua");
    return exporter.exportToFile(outputPath);
}

bool LuaExportHelper::quickExport(QWidget* widget, const QString& processName)
{
    QString moduleName = "UnknownModule";  // Could try to detect from widget tree
    return exportInterface(widget, moduleName, processName, "./exported");
}
