#pragma once

#include <QObject>
#include <QString>

#include <Controllers/RotationBodyPlanningController.h>

namespace smrobot::workbench::spray::rotationbody
{
    class ABBTranslationPanel;
    class PostProcessingPanel;
    class RotationBodyPlanningController;
    class WorkpieceCalibrationPanel;

    class CalibrationTranslationModuleController final : public QObject
    {
        Q_OBJECT

    public:
        CalibrationTranslationModuleController(
            RotationBodyPlanningController& controller,
            WorkpieceCalibrationPanel& calibrationPanel,
            ABBTranslationPanel& abbPanel,
            PostProcessingPanel& postProcessingPanel,
            QObject* parent = nullptr);

        void setLanguageCode(const QString& languageCode);
        QString languageCode() const;
        void refresh();

    signals:
        void statusMessageRequested(const QString& message, int timeoutMs);

    private:
        void report(const RotationBodyControllerResult& result, int timeoutMs = 3500);

        RotationBodyPlanningController& m_controller;
        WorkpieceCalibrationPanel& m_calibrationPanel;
        ABBTranslationPanel& m_abbPanel;
        PostProcessingPanel& m_postProcessingPanel;
        QString m_languageCode{ QStringLiteral("en") };
        bool m_updating{ false };
    };
}
