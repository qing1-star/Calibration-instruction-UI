#include "CalibrationTranslationModuleController.h"

#include <Controllers/RotationBodyPlanningController.h>
#include <Models/RotationBodyPlanningTranslations.h>
#include <Widgets/ABBTranslationPanel.h>
#include <Widgets/PostProcessingPanel.h>
#include <Widgets/WorkpieceCalibrationPanel.h>

#include <QFileInfo>

namespace smrobot::workbench::spray::rotationbody
{
    CalibrationTranslationModuleController::CalibrationTranslationModuleController(
        RotationBodyPlanningController& controller,
        WorkpieceCalibrationPanel& calibrationPanel,
        ABBTranslationPanel& abbPanel,
        PostProcessingPanel& postProcessingPanel,
        QObject* parent)
        : QObject(parent)
        , m_controller(controller)
        , m_calibrationPanel(calibrationPanel)
        , m_abbPanel(abbPanel)
        , m_postProcessingPanel(postProcessingPanel)
    {
        connect(&m_controller, &RotationBodyPlanningController::stateChanged,
            this, &CalibrationTranslationModuleController::refresh);
        connect(&m_calibrationPanel, &WorkpieceCalibrationPanel::workspaceEdited,
            this, [this](const WorkpieceCalibrationWorkspace& workspace) {
                if(m_updating) return;
                report(m_controller.updateCalibrationWorkspace(workspace));
            });
        connect(&m_calibrationPanel, &WorkpieceCalibrationPanel::modeTwoDataImported,
            this, [this](const Eigen::Vector3d& safetyPositionBaseMeters,
                const QString& filePath) {
                domain::RapidExportSettings settings =
                    m_controller.viewModel().rapidSettings;
                settings.safetyPositionBaseMeters = safetyPositionBaseMeters;
                RotationBodyControllerResult result =
                    m_controller.updateRapidSettings(settings);
                if(result.success) {
                    result.message = RotationBodyPlanningTranslations::text(
                        m_languageCode, "calibration.import_success")
                        .arg(QFileInfo(filePath).fileName());
                }
                report(result, 4000);
            });
        connect(&m_calibrationPanel,
            &WorkpieceCalibrationPanel::calculateWorkpieceFrameRequested,
            this,
            [this]() { report(m_controller.calculateAndApplyWorkpieceFrame(), 3500); });
        connect(&m_calibrationPanel, &WorkpieceCalibrationPanel::publishFrameChanged,
            this, [this](PublishFrame frame) {
                m_controller.setPublishFrame(frame);
                refresh();
            });
        connect(&m_abbPanel, &ABBTranslationPanel::settingsEdited,
            this, [this](const domain::RapidExportSettings& settings) {
                report(m_controller.updateRapidSettings(settings), 2500);
            });
        connect(&m_abbPanel, &ABBTranslationPanel::sequenceEdited,
            this, [this](const std::vector<domain::RapidSequenceEntry>& sequence) {
                report(m_controller.updateRapidSequence(sequence), 2500);
            });
        connect(&m_abbPanel, &ABBTranslationPanel::generateRequested,
            this, [this](const domain::RapidExportSettings& settings,
                const std::vector<domain::RapidSequenceEntry>& sequence) {
                RotationBodyControllerResult result = m_controller.updateRapidSettings(settings);
                if(result.success) result = m_controller.updateRapidSequence(sequence);
                if(result.success) result = m_controller.generateAndSaveRapidModule();
                report(result, 5000);
            });
        connect(&m_abbPanel, &ABBTranslationPanel::previewStepSelected,
            this, [this](int index) {
                m_controller.setSelectedRapidPreviewStep(index >= 0
                    ? std::optional<std::size_t>(static_cast<std::size_t>(index))
                    : std::nullopt);
            });
        connect(&m_postProcessingPanel,
            &PostProcessingPanel::loadCurrentTrajectoryRequested,
            this,
            [this](const QString& robotId) {
                report(m_controller.loadCurrentTrajectoryForPostProcessing(
                    robotId.toUtf8().toStdString()), 6000);
            });
        connect(&m_postProcessingPanel,
            &PostProcessingPanel::generateRequested,
            this,
            [this](const QString& templateName,
                const QString& robotId,
                const QString& programName,
                const QString& outputDirectory) {
                report(m_controller.generatePostProcessedProgram(
                    templateName.toUtf8().toStdString(),
                    robotId.toUtf8().toStdString(),
                    programName.toUtf8().toStdString(),
                    outputDirectory.toUtf8().toStdString()), 6000);
            });
        refresh();
    }

    void CalibrationTranslationModuleController::setLanguageCode(const QString& languageCode)
    {
        m_languageCode = RotationBodyPlanningTranslations::canonicalLanguageCode(languageCode);
        m_calibrationPanel.setLanguageCode(m_languageCode);
        m_abbPanel.setLanguageCode(m_languageCode);
        m_postProcessingPanel.setLanguageCode(m_languageCode);
        refresh();
    }

    QString CalibrationTranslationModuleController::languageCode() const
    {
        return m_languageCode;
    }

    void CalibrationTranslationModuleController::refresh()
    {
        m_updating = true;
        const RotationBodyPlanningViewModel viewModel = m_controller.viewModel();
        m_calibrationPanel.setViewModel(viewModel);
        m_abbPanel.setViewModel(viewModel);
        m_postProcessingPanel.setViewModel(viewModel);
        m_updating = false;
    }

    void CalibrationTranslationModuleController::report(
        const RotationBodyControllerResult& result,
        int timeoutMs)
    {
        refresh();
        if(!result.message.isEmpty()) {
            emit statusMessageRequested(result.message, timeoutMs);
        }
    }
}
