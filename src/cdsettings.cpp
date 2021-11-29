/*!
 * \file csettings.cpp
 *
 * \author David Smejkal
 * \date 27.4.2009
 *
 * cDSettings implements dialog from settings.ui which is created by Qt Designer
 * (graphical tool for composing GUI).
 */

#include "cdsettings.h"
#include "cmainwindow.h"

/*!
 * \brief Constructor of cDSettings.
 *
 * Saving pointer on parent and creating dialog from settings.ui
 *
 */
cDSettings::cDSettings(cMainWindow *parent)
    : QDialog(parent)
{
    parentCWidget = parent;
    setupUi(this); // create widget from ui file

    setSettingsDialog();

    /* checkBoxes autoexlusivity and disable */
    connect(this->checkBox_multisampling, SIGNAL(toggled(bool)),\
            this, SLOT(unCheckAA(bool)));
    connect(this->checkBox_antialiasing, SIGNAL(toggled(bool)),\
            this, SLOT(unCheckMS(bool)));
    /* buttonBox_settings */
    connect(this->buttonBox_settings, SIGNAL(accepted()),\
            this, SLOT(acceptSettings()));
    connect(this->buttonBox_settings, SIGNAL(rejected()),\
            this, SLOT(rejectSettings()));
}

cDSettings::~cDSettings()
{
    parentCWidget->settings_recreateGL = false;
//    this->parentCWidget->writeSettings();
}

/*!
 * \brief Method for unchecking antialiasing checkbox.
 *
 * If state is true then unchek antialiasing checkBox.
 *
 * \note slot
 */
void cDSettings::unCheckAA(bool state)
{
    if(state)
    {
        this->checkBox_antialiasing->setChecked(false);
        //this->horizontalSlider_antialiasing_all->setValue(this->horizontalSlider_antiAliasing_all->value());
    }
}

/*!
 * \brief Method for unchecking multisampling checkbox.
 *
 * If state is true then unchek multisampling checkBox.
 *
 * \note slot
 */
void cDSettings::unCheckMS(bool state)
{
    if(state)
    {
        this->checkBox_multisampling->setChecked(false);
        //this->horizontalSlider_antialiasing_all->setValue(this->horizontalSlider_antiAliasing_all->value());
    }
}

/*!
 * \brief Method binded on pressing ok in settings dialog.
 *
 * First evaluate settings (this will set some variables) then write settings.
 * And finaly close the dialog.
 *
 * \note slot
 */
void cDSettings::acceptSettings()
{

    evalSettings();
    this->parentCWidget->writeSettings();
    this->hide();
}

/*!
 * \brief Method binded on pressing cancel in settings dialog.
 *
 * Just prevent recreation of OpenGL kontext and close the dialog.
 *
 * \note slot
 */
void cDSettings::rejectSettings()
{
    parentCWidget->settings_recreateGL = false;
    this->hide();
}

/*!
 * \brief Method for getting parameters from settings dialog.
 *
 * This method is called in slot that accepts settings.
 */
void cDSettings::evalSettings()
{
    parentCWidget->settings_recreateGL = false;

    // Anti-aliasing checkbox
    if(checkBox_antialiasing->isChecked())
    {
        parentCWidget->settings_antialiasing = hSlider_antialiasing->value();
    } else
    {
        parentCWidget->settings_antialiasing = 0;
    }

    if(checkBox_multisampling->isChecked())
    {
        if(parentCWidget->settings_multisampling != \
           hSlider_multisampling->value())
        {
            parentCWidget->settings_recreateGL = true;
            parentCWidget->settings_multisampling = \
                hSlider_multisampling->value();
        } else parentCWidget->settings_recreateGL = false;
    } else
    {
        if(parentCWidget->settings_multisampling != 0)
        {
            parentCWidget->settings_recreateGL = true;
            parentCWidget->settings_multisampling = 0;
        } else parentCWidget->settings_recreateGL = false;
    }

    if(checkBox_navigation->isChecked())
    {
        parentCWidget->settings_navigation = true;
    } else
    {
        parentCWidget->settings_navigation = false;
    }

    if(checkBox_navigation->isChecked())
    {
        parentCWidget->settings_navigation = true;
    } else
    {
        parentCWidget->settings_navigation = false;
    }

    if(radioButton_smallship->isChecked())
    {
        if(parentCWidget->settings_object != QString("small_ship.obj"))
            parentCWidget->settings_recreateGL = true;
        parentCWidget->settings_object = QString("small_ship.obj");
    } else if(radioButton_alienship->isChecked())
    {
        if(parentCWidget->settings_object != QString("alien_ship.obj"))
            parentCWidget->settings_recreateGL = true;
        parentCWidget->settings_object = QString("alien_ship.obj");
    } else if(radioButton_ownship->isChecked())
    {
        if(parentCWidget->settings_object != QString("own_ship.obj"))
            parentCWidget->settings_recreateGL = true;
        parentCWidget->settings_object = QString("own_ship.obj");
    } else
    {
        if(parentCWidget->settings_object != QString("small_ship.obj"))
            parentCWidget->settings_recreateGL = true;
        parentCWidget->settings_object = QString("small_ship.obj");
    }

    if(radioButton_triangles->isChecked())
    {
        parentCWidget->settings_polygons = 0;
    } else if(radioButton_quads->isChecked())
    {
        parentCWidget->settings_polygons = 1;
    } else
    {
        parentCWidget->settings_polygons = 0;
    }
}

/*!
 * \brief Method that is called before each popup of settings dialog.
 *
 * Sets dialog widgets according to settings parameters in cMainWindow.
 */
void cDSettings::setSettingsDialog()
{
    //parentCWidget->readSettings();

    if(parentCWidget->settings_multisampling)
    {
        checkBox_multisampling->setChecked(true);
        hSlider_multisampling->setValue(parentCWidget->settings_multisampling);
    } else
    {
        checkBox_antialiasing->setChecked(false);
        hSlider_antialiasing->setValue(0);
    }

    if(parentCWidget->settings_antialiasing)
    {
        checkBox_antialiasing->setChecked(true);
        hSlider_antialiasing->setValue(parentCWidget->settings_antialiasing);
    } else
    {
        checkBox_antialiasing->setChecked(false);
        hSlider_antialiasing->setValue(0);
    }

    if(parentCWidget->settings_navigation)
    {
        checkBox_navigation->setChecked(true);
    } else
    {
        checkBox_navigation->setChecked(false);
    }

    if(parentCWidget->settings_object == QString("small_ship.obj"))
    {
        radioButton_ownship->setChecked(false);
        radioButton_alienship->setChecked(false);
        radioButton_smallship->setChecked(true);
    } else if(parentCWidget->settings_object == QString("alien_ship.obj"))
    {
        radioButton_smallship->setChecked(false);
        radioButton_ownship->setChecked(false);
        radioButton_alienship->setChecked(true);
    } else if(parentCWidget->settings_object == QString("own_ship.obj"))
    {
        radioButton_smallship->setChecked(false);
        radioButton_alienship->setChecked(false);
        radioButton_ownship->setChecked(true);
    } else
    {
        radioButton_alienship->setChecked(false);
        radioButton_ownship->setChecked(false);
        radioButton_smallship->setChecked(true);
    }

    if(parentCWidget->settings_polygons == 0)
    {
        radioButton_triangles->setChecked(true);
        radioButton_quads->setChecked(false);
    } else
    {
        radioButton_triangles->setChecked(false);
        radioButton_quads->setChecked(true);
    }
}

 void cDSettings::closeEvent(QCloseEvent *event)
 {
    parentCWidget->settings_recreateGL = false;
 }










