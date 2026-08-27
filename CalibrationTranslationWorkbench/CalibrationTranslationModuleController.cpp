#include "CalibrationTranslationModuleController.h"

#include <Controllers/RotationBodyPlanningController.h>
#include <Models/RotationBodyPlanningTranslations.h>
#include <Widgets/ABBTranslationPanel.h>
#include <Widgets/WorkpieceCalibrationPanel.h>

namespace smrobot::workbench::spray::rotationbody
{
    CalibrationTranslationModuleController::CalibrationTranslationModuleController(
        RotationBodyPlanningController& controller,
        WorkpieceCalibrationPanel& calibrationPanel,
        ABBTranslationPanel& abbPanel,
        QObject* parent)
        : QObject(parent)
        , m_controller(controller)
        , m_calibrationPanel(calibrationPanel)
        , m_abbPanel(abbPanel)
    {
        connect(&m_controller, &RotationBodyPlanningController::stateChanged,
            this, &CalibrationTranslationModuleController::refresh);
        connect(&m_calibrationPanel, &WorkpieceCalibrationPanel::workspaceEdited,
            this, [this](const WorkpieceCalibrationWorkspace& workspace) {
                if(m_updating) return;
                report(m_controller.updateCalibrationWorkspace(workspace));
            });
        connect(&m_calibrationPanel, &WorkpieceCalibrationPanel::baseTransformCalculated,
            this, [this](const domain::TransformComponents& components) {
                report(m_controller.updateBaseComponents(components), 3500);
            });
        connect(&m_calibrationPanel, &WorkpieceCalibrationPanel::publishFrameChanged,
            this, [this](PublishFrame frame) {
                m_controller.setPublishFrame(frame);
                refresh();
            });
        connect(&m_calibrationPanel, &WorkpieceCalibrationPanel::confirmFrameRequested,
            this, [this]() { report(m_controller.confirmFrame(), 3500); });
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
        refresh();
    }

    void CalibrationTranslationModuleController::setLanguageCode(const QString& languageCode)
    {
        m_languageCode = RotationBodyPlanningTranslations::canonicalLanguageCode(languageCode);
        m_calibrationPanel.setLanguageCode(m_languageCode);
        m_abbPanel.setLanguageCode(m_languageCode);
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
