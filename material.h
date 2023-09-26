/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Material.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 14th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef MATERIAL_H
#define MATERIAL_H

#include <QDialog>
#include <QDoubleValidator>
#include <QItemSelectionModel>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItemModel>

#include "manager.h"
#include "rename.h"

/*  ############################################################################
 *  class Material: define the Material object, which is used to define the
 *  material property with respect to density, elastic modlus, neo-Hookean
 *  mateiral, thermal conduction and thermal expansion and so on  */
namespace Ui {
class Material;
}

class Material : public QDialog {
    Q_OBJECT
private:
    Ui::Material* ui;  // UI model

private:
    //  property menu
    QMenu* general;      // menu for general properties
    QMenu* mechanical;   // menu for mechanical properties
    QMenu* thermal;      // menu for thermal properties
    QMenu* elecMag;      // menu for eleectical/magnetic properties
    QMenu* other;        // menu for other properties
    QString btnstyle;    // menu button style

    QMessageBox msgbox;  // message box to show the information

    class Property;      // the class of material property
    Property* proNew;    // the newly created property
    Property* proOld;    // the old material property
    Property* proCur;    // current material property for operation

    //  other needed varaibles
    int index;                         // the current index of material property
    bool editmode;                     // if the material is in edit mode
    Manager* mng;                      // the material manager
    Rename* rename;                    // the rename dialog
    bool isActiveMng;                  // is the manager is actived
    QDoubleValidator* validator;       // formator
    QStandardItem* item;               // the item in list view
    QStandardItemModel* itemPro;       // view list items for property
    QStandardItemModel* itemModel;     // view list items for manager
    QItemSelectionModel* selectModel;  // view list selection model

private:
    QMap<QString, int> map = {{"Density", 0},
                              {"Linear Material", 1},
                              {"Neo-Hookean Material", 2},
                              {"Thermal Expansion", 3},
                              {"Thermal Conduction", 4}

    };

    /*  Item configuration */
    QAction* actDen;      // material density
    QAction* actLinear;   // linear material
    QAction* actNeo;      // neo-Hookean mateiral
    QAction* actExpan;    // thermal expansion
    QAction* actConduct;  // thermal conduction

public:
    /*  constructor: create the material object  */
    explicit Material(QWidget* parent = nullptr);

    /*  destructor: destroy the material object  */
    ~Material();

private:
    /*  setup material properties UI information  */
    void setupGeneralPro();     // material density
    void setupMechanicalPro();  // linear material
    void setupThermoPro();      // thermal properties
    void setupElecMag();        // electromagnetic
    void setupOthers();         // others
    void setupManager();        // material manager object
    void setupRename();         // rename dialog

    /*  setupMatProListview: create the listview model for the created material
     * properties*/
    void setupMatProListview();

    /*  reset: reset the dialog to the default  */
    void reset();

private slots:
    /*  Properties listview  */
    void addItemDen();     // density
    void addItemLinear();  // linear material
    void addItemNeo();     // neo-hookean material
    void addItemThermo();  // thermal expansion

    /*  switch between elastic and bulk  */
    void setLinearElastic();  // use linear elastic modulus
    void setLinearBulk();     // use linear bulk modulus
    void setNeoElastic();     // use neo-Hookean elastic modulus
    void setNeoBulk();        // use neo-Hookean bulk modulus

    /*  set status of thermal conduction and expansion  */
    void setExpansion(int flag);
    void setConduction(int flag);

    //  delete the select material item
    void delSelectedMatProItem();  // delete the selected item
    void resetDensity();           // reset density property
    void resetLinear();            // reset Linear material
    void resetNeo();               // reset Neo-Hookean material
    void resetConduct();           // reset thermal conduction
    void resetExpan();             // reset thermal expansion
    void discard();                // discard the current material page

    //  manager related interface
    void edit(int idx);        // load the information of selected material
    void setOldName(int idx);  // set the original name of material
    void getNewName();         // set the new name to the matrial
    void remove(int idx);      // delete selected material

    /*  saveMaterial: save the material properties  */
    void saveMaterial();

public:
    /*  interface for the outside involkation  */
    void create();   //  create a material
    void manager();  // show the mananager

protected:
    /*  closeEvent: override the close event  */
    void closeEvent(QCloseEvent* event) override;
    /*  showEvent: override the show event  */
    void showEvent(QShowEvent* event) override;
};

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  class Property: the class used to define the material property */
class Material::Property {
public:
    //  basic configuration
    int index;        // inner index of the current material
    QString name;     // name of material
    QString descrip;  // description of material
    bool assigned;    // if the current material property is assigned

public:
    /*  Config: nested class for the detailed configuration of each material
     *  properties*/
    class Config {
    public:
        //  basic configuration
        double* data;   // data of the property item
        bool assigned;  // if this property is assigned
        int* flag;      // some configuration current material property

    public:
        /*  constructor: create the config object
            @argc  _name: name of the configuration
            @argc  _numData: number of data information
            @argc  _numFlag: number of configuration flag  */
        Config(const QString& _name, const int& _numData, const int& _numFlag);

        /*  destructor: destroy the config object  */
        ~Config();

        /*  update the assign status  */
        void updateAssign(const bool status) { assigned = status; }
    };
    /*  configuration object for each material property  */
    Config* den;      // material density
    Config* linear;   // linear material
    Config* neo;      // neo-Hookean material
    Config* expan;    // thermal expansion
    Config* conduct;  // thermal conduction

public:
    /*  constructor:  create the material property object
        @argc  _index: the inner order of the current material  */
    Property(const int& _index);

    /*  destructor: destroy the material property object  */
    ~Property();
};

#endif  // MATERIAL_H
