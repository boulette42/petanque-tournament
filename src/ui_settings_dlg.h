/********************************************************************************
** Form generated from reading UI file 'settings_dlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_DLG_H
#define UI_SETTINGS_DLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *gbMode;
    QHBoxLayout *horizontalLayout;
    QRadioButton *rbSuperMelee;
    QRadioButton *rbTeams;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *tlDataDir;
    QLineEdit *leDataDir;
    QToolButton *tbSelectDir;
    QHBoxLayout *horizontalLayout_3;
    QLabel *tlFontSize;
    QLineEdit *leFontSize;
    QLabel *tlPoint;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *cbSiteEnabled;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QString::fromUtf8("SettingsDialog"));
        SettingsDialog->resize(470, 231);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gbMode = new QGroupBox(SettingsDialog);
        gbMode->setObjectName(QString::fromUtf8("gbMode"));
        horizontalLayout = new QHBoxLayout(gbMode);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        rbSuperMelee = new QRadioButton(gbMode);
        rbSuperMelee->setObjectName(QString::fromUtf8("rbSuperMelee"));

        horizontalLayout->addWidget(rbSuperMelee);

        rbTeams = new QRadioButton(gbMode);
        rbTeams->setObjectName(QString::fromUtf8("rbTeams"));

        horizontalLayout->addWidget(rbTeams);

        horizontalSpacer = new QSpacerItem(210, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(gbMode);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        tlDataDir = new QLabel(SettingsDialog);
        tlDataDir->setObjectName(QString::fromUtf8("tlDataDir"));
        tlDataDir->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(tlDataDir);

        leDataDir = new QLineEdit(SettingsDialog);
        leDataDir->setObjectName(QString::fromUtf8("leDataDir"));

        horizontalLayout_2->addWidget(leDataDir);

        tbSelectDir = new QToolButton(SettingsDialog);
        tbSelectDir->setObjectName(QString::fromUtf8("tbSelectDir"));

        horizontalLayout_2->addWidget(tbSelectDir);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        tlFontSize = new QLabel(SettingsDialog);
        tlFontSize->setObjectName(QString::fromUtf8("tlFontSize"));

        horizontalLayout_3->addWidget(tlFontSize);

        leFontSize = new QLineEdit(SettingsDialog);
        leFontSize->setObjectName(QString::fromUtf8("leFontSize"));

        horizontalLayout_3->addWidget(leFontSize);

        tlPoint = new QLabel(SettingsDialog);
        tlPoint->setObjectName(QString::fromUtf8("tlPoint"));

        horizontalLayout_3->addWidget(tlPoint);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_3);

        cbSiteEnabled = new QCheckBox(SettingsDialog);
        cbSiteEnabled->setObjectName(QString::fromUtf8("cbSiteEnabled"));

        verticalLayout->addWidget(cbSiteEnabled);

        verticalSpacer = new QSpacerItem(20, 18, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#if QT_CONFIG(shortcut)
        tlDataDir->setBuddy(leDataDir);
#endif // QT_CONFIG(shortcut)

        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Einstellungen", nullptr));
        gbMode->setTitle(QCoreApplication::translate("SettingsDialog", "Modus", nullptr));
        rbSuperMelee->setText(QCoreApplication::translate("SettingsDialog", "Supermel\303\251e", nullptr));
        rbTeams->setText(QCoreApplication::translate("SettingsDialog", "Feste Teams", nullptr));
        tlDataDir->setText(QCoreApplication::translate("SettingsDialog", "Daten-Verzeichnis", nullptr));
        tbSelectDir->setText(QCoreApplication::translate("SettingsDialog", "...", nullptr));
        tlFontSize->setText(QCoreApplication::translate("SettingsDialog", "Font-Gr\303\266\303\237e", nullptr));
        tlPoint->setText(QCoreApplication::translate("SettingsDialog", "pt", nullptr));
        cbSiteEnabled->setText(QCoreApplication::translate("SettingsDialog", "Pl\303\244tze ber\303\274cksichtigen", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_DLG_H
