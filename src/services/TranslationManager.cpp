#include "TranslationManager.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QTranslator>

TranslationManager::TranslationManager(QApplication *application, QObject *parent)
    : QObject(parent)
    , m_application(application)
    , m_translator(new QTranslator(this))
{
}

bool TranslationManager::switchLanguage(const QString &language)
{
    const QString normalizedLanguage = language.trimmed().toLower();
    if (normalizedLanguage != QStringLiteral("en")
        && normalizedLanguage != QStringLiteral("ru")
        && normalizedLanguage != QStringLiteral("de")) {
        return false;
    }

    if (m_application == nullptr) {
        return false;
    }

    m_application->removeTranslator(m_translator);
    m_translator->load(QString());

    if (normalizedLanguage != QStringLiteral("en")) {
        const QString fileName = translationFileName(normalizedLanguage);
        const QString resourcePath = QStringLiteral(":/translations/%1").arg(fileName);
        const QString fileSystemPath = QCoreApplication::applicationDirPath()
                                       + QDir::separator()
                                       + QStringLiteral("translations")
                                       + QDir::separator()
                                       + fileName;

        if (m_translator->load(resourcePath) || m_translator->load(fileSystemPath)) {
            m_application->installTranslator(m_translator);
        }
    }

    m_currentLanguage = normalizedLanguage;
    emit languageChanged(m_currentLanguage);
    return true;
}

QString TranslationManager::currentLanguage() const
{
    return m_currentLanguage;
}

QString TranslationManager::translationFileName(const QString &language) const
{
    return QStringLiteral("office_rooms_%1.qm").arg(language);
}
