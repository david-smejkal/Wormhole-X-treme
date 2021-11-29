/*!
 * \file cdsettings.h
 *
 * \author David Smejkal
 * \date 27.4.2009
 *
 * cDSettings implements dialog from settings.ui which is created by Qt Designer
 * (graphical tool for composing GUI).
 */

#ifndef CDSETTINGS_H
#define CDSETTINGS_H


#include "ui_settings.h"

class cMainWindow;

/*!
 * \class cDSettings
 * \brief Wrapper for settings.ui, created by Qt Designer.
 *
 * First we need to let this class inherit Ui::dialog_settings then
 * setupUi(this) can be called from this class's constructor and gui will get
 * wrapped in this class.
 */
class cDSettings : public QDialog, private Ui::dialog_settings
{
    Q_OBJECT

public:
    cDSettings(cMainWindow *parent = 0);
    ~cDSettings();

    void setSettingsDialog(); // used befor opening settings dialog

private slots:
    void acceptSettings();
    void rejectSettings();
    void unCheckAA(bool state);
    void unCheckMS(bool state);


private:
    void evalSettings();
    void closeEvent(QCloseEvent *event);
    cMainWindow * parentCWidget;
};

#endif // CDSETTINGS_H
