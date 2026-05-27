#pragma once

#include <QObject>
#include <QString>

class QApplication;
class QTranslator;

/**
 * @brief Manages dynamic interface translation loading.
 *
 * Loads ru, en and de translations from Qt resource files or translation folders.
 */
class TranslationManager final : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates a translation manager.
     * @param application Application object that owns installed translators.
     * @param parent Parent QObject.
     */
    explicit TranslationManager(QApplication *application, QObject *parent = nullptr);

    /**
     * @brief Switches application language dynamically.
     * @param language Language code: en, ru or de.
     * @return True if requested language was applied.
     */
    bool switchLanguage(const QString &language);

    /**
     * @brief Returns current language code.
     * @return Current language code.
     */
    QString currentLanguage() const;

signals:
    /**
     * @brief Emitted after language was changed.
     * @param language New language code.
     */
    void languageChanged(const QString &language);

private:
    /**
     * @brief Builds Qt translation file name for a language.
     * @param language Language code.
     * @return Translation file name.
     */
    QString translationFileName(const QString &language) const;

    QApplication *m_application;
    QTranslator *m_translator;
    QString m_currentLanguage = QStringLiteral("en");
};
