
#include "interface/widgets/SettingsWidget.h"

#include <Settings/Settings.h>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "interface/widgets/mainWindow.h"

namespace rtt::ai::interface {

SettingsWidget::SettingsWidget(QWidget* parent) {
    vLayout = new QVBoxLayout();
    this->setLayout(vLayout);

    // grsim ip + port settings
    QGroupBox* grsimSettingsGroup = new QGroupBox("grsim transmission ip + port");
    auto grsimSettingsWidgetLayout = new QHBoxLayout();
    grsimIpText = new QLineEdit();
    grsimIpText->setText(QString::fromStdString(SETTINGS.getRobothubSendIp()));
    QObject::connect(grsimIpText, SIGNAL(textChanged(QString)), this, SLOT(changeGrSimIp(QString)));
    grsimSettingsWidgetLayout->addWidget(grsimIpText);
    grsimPort = new QSpinBox();
    grsimPort->setRange(0, 999999);
    grsimPort->setValue(SETTINGS.getRobothubSendPort());
    grsimSettingsWidgetLayout->addWidget(grsimPort);
    grsimSettingsGroup->setLayout(grsimSettingsWidgetLayout);
    QObject::connect(grsimPort, SIGNAL(textChanged(QString)), this, SLOT(changeGrSimPort(int)));
    vLayout->addWidget(grsimSettingsGroup);

    auto spacer = new QSpacerItem(100, 100, QSizePolicy::Expanding, QSizePolicy::Expanding);
    vLayout->addSpacerItem(spacer);
}

void SettingsWidget::changeTeamColor(bool isYellow) { SETTINGS.setYellow(isYellow); }

void SettingsWidget::changeTeamSide(bool isLeft) { SETTINGS.setLeft(isLeft); }

void SettingsWidget::changeMode(bool serial) { SETTINGS.setSerialMode(serial); }

void SettingsWidget::changeGrSimIp(QString ip) {
    std::cout << "setting grsimip" << std::endl;
    SETTINGS.setRobothubSendIp(ip.toStdString());
}

void SettingsWidget::changeGrSimPort(int port) {
    std::cout << "setting grsimport" << std::endl;
    SETTINGS.setRobothubSendPort(port);
}

}  // namespace rtt::ai::interface

// QT performance improvement
#include "include/roboteam_ai/interface/widgets/moc_SettingsWidget.cpp"
