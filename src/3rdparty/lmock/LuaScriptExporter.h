#pragma once

#include <QWidget>
#include <QTextStream>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QDateTime>

struct LuaExportOptions
{
    QString moduleName = "UnknownModule";
    QString processName = "UnknownProcess";
    QString scriptName;  // Auto-generated from processName if empty
    bool includeComments = true;
    bool includeMetadata = true;
    bool groupParameters = false;  // Group into logical sections
};

struct LuaParameter
{
    QString name;           // Parameter name (camelCase)
    QString type;           // "number", "boolean", "string", "table"
    QString value;          // Lua value representation
    QString controlType;    // Source control type (Slider, CheckBox, etc.)
    QString controlId;      // Qt objectName
    QString comment;        // Optional comment
    QString group;          // Optional group name
};

/**
 * Export Qt widget tree as executable Lua automation script
 * 
 * Usage:
 *   LuaScriptExporter exporter(rootWidget, options);
 *   exporter.writeLuaScript(outputStream);
 */
class LuaScriptExporter
{
public:
    explicit LuaScriptExporter(QWidget* rootWidget, 
                              const LuaExportOptions& opt);
    
    // Main export method
    void writeLuaScript(QTextStream& out);
    
    // Export to file
    bool exportToFile(const QString& filePath);
    
    // Get extracted parameters (for inspection)
    QList<LuaParameter> parameters() const { return m_parameters; }
    
    // Static convenience methods
    static bool exportWidget(QWidget* widget, 
                            const QString& moduleName,
                            const QString& processName,
                            const QString& outputPath);

private:
    QWidget* m_root;
    LuaExportOptions m_opt;
    QList<LuaParameter> m_parameters;
    QStringList m_traversalLog;  // ADD THIS
    
    // Parameter extraction
    void extractParameters();
    void extractFromWidget(QWidget* widget, const QString& groupName = QString());
    
    // Widget value extraction
    QString extractValue(QWidget* widget, const QString& controlType);
    QString determineLuaType(QWidget* widget, const QString& controlType);
    QString sanitizeParameterName(const QString& name);
    
    // Script generation
    void writeHeader(QTextStream& out);
    void writeParameterTable(QTextStream& out);
    void writeExecuteFunction(QTextStream& out);
    void writeFooter(QTextStream& out);
    void writeMetadataComment(QTextStream& out);
    
    // Grouping helpers
    void detectGroups();
    QString detectGroup(const QString& objectName);
};

/**
 * Helper to export from ExportHelper-style convenience functions
 */
class LuaExportHelper
{
public:
    static bool exportInterface(QWidget* widget,
                               const QString& moduleName,
                               const QString& processName,
                               const QString& outputDir = "./exported");
    
    static bool quickExport(QWidget* widget,
                           const QString& processName);
};
