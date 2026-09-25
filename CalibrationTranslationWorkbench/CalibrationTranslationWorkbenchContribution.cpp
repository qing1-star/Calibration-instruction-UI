#include "CalibrationTranslationWorkbenchContribution.h"

namespace robot_qt_viewer
{
    bool registerCalibrationTranslationWorkbenchContribution(
        RobotQtViewerWorkbenchPackageRegistry& catalog)
    {
        const QString packageId = QStringLiteral("smrobot.workbench.calibration-translation");
        RobotQtViewerWorkbenchDescriptor descriptor;
        descriptor.kind = RobotQtViewerWorkbenchKind::Browse;
        descriptor.domain = RobotQtViewerWorkbenchDomain::SprayProcess;
        descriptor.rightPanel = RobotQtViewerRightPanelKind::Status;
        descriptor.defaultViewportMode = RobotQtViewerViewportInteractionMode::Browse;
        descriptor.id = QStringLiteral("smrobot.mode.calibration-translation");
        descriptor.displayName = QStringLiteral("Calibration & ABB Translation");
        descriptor.rightPanelTitle = QStringLiteral("Calibration & ABB Translation");
        descriptor.projectAssemblyTreeProjection = false;
        descriptor.projectAssemblyActions = false;

        return catalog.registerPackage(makeRobotQtViewerWorkbenchPackage(
                   packageId, QStringLiteral("Calibration & ABB Translation"))) &&
            catalog.registerWorkbench(makeRobotQtViewerWorkbench(
                packageId,
                descriptor,
                QStringLiteral("calibrationTranslationWorkbench"),
                66,
                { QStringLiteral("smrobot.feature.calibration-translation") },
                { QStringLiteral("smrobot.mode.rotation-body-planning") }));
    }
}
