#ifndef __APPCONFIG_H__
#define __APPCONFIG_H__

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QDebug>

class QRect;
class QStringList;

class AppConfig : public QObject {
    Q_OBJECT

public:
    AppConfig(QObject *pobj = 0);
    ~AppConfig();

    void init(void);
    bool is_init(void);

    void sync(void);

    // Версия формата конфигфайла
    int get_config_version(void)const;
    void set_config_version(int i);

    // Рабочая директория
    QString get_tetradir(void)const;
    bool set_tetradir(QString dirName);

    // Директория с корзиной
    QString get_trashdir(void)const;
    bool set_trashdir(QString dirName);

    // Максимальный размер директории корзины в мегабайтах
    unsigned int get_trashsize(void)const;
    bool set_trashsize(unsigned int mbSize);

    // Максимально допустимое число файлов в корзине
    int get_trashmaxfilecount(void)const;
    bool set_trashmaxfilecount(int count);

    // Нужно ли показывать подтверждение при выполнении действия "cut на ветке
    bool get_cutbranchconfirm(void)const;
    bool set_cutbranchconfirm(bool confirm);

    // Нужно ли печатать отладочные сообщения в консоль
    bool get_printdebugmessages(void)const;
    bool set_printdebugmessages(bool isPrint);

    // Язык интерфейса
    QString get_interfacelanguage(void)const;
    bool set_interfacelanguage(QString language);

    // Номер последнего префикса в виде строки с ведущими нулями
    QString get_addnewrecord_expand_info(void)const;
    void set_addnewrecord_expand_info(QString);

    // Геометрия основного окна
    QRect mainwin_geometry(void)const;
    void mainwin_geometry(int x, int y, int w, int h);

    //    QSize treescreensize(void)const;
    //    void treescreensize(const QSize&);

    void v_leftsplitter_sizelist(QList<int> list);
    QList<int> v_leftsplitter_sizelist();

    QList<int> vspl_sizelist(void)const;
    void vspl_sizelist(QList<int> list);

    QList<int> hspl_sizelist(void)const;
    void hspl_sizelist(QList<int> list);

    QList<int> findsplitter_sizelist(void)const;
    void findsplitter_sizelist(QList<int> list);

    QList<int> splitter_sizelist(QString name)const;
    void splitter_sizelist(QString name, QList<int> list);


    QStringList get_tree_position(void)const;
    void set_tree_position(QStringList list);


    // ID записи в таблице конечных записей, которая выделена на экране
    QString get_recordtable_selected_record_id(void)const;
    void set_recordtable_selected_record_id(QString id);


    // Настройки виджета поиска по базе
    int get_findscreen_wordregard(void)const;
    void set_findscreen_wordregard(int pos);

    int get_findscreen_howextract(void)const;
    void set_findscreen_howextract(int pos);

    int getFindScreenTreeSearchArea(void)const;
    void setFindScreenTreeSearchArea(int pos);

    bool get_findscreen_find_in_field(QString fieldName)const;
    void set_findscreen_find_in_field(QString fieldName, bool isChecked);

    bool get_editor_show(void)const;
    void set_editor_show(bool isShow);

    bool get_findscreen_show(void)const;
    void set_findscreen_show(bool isShow);

    QString get_howpassrequest(void)const;
    void set_howpassrequest(QString mode);

    bool get_runinminimizedwindow(void)const;
    void set_runinminimizedwindow(bool flag);

    bool get_autoClosePasswordEnable(void)const;
    void set_autoClosePasswordEnable(bool flag);

    int  get_autoClosePasswordDelay(void)const;
    void set_autoClosePasswordDelay(int delay);

    QString get_synchrocommand(void)const;
    void set_synchrocommand(QString command);

    bool get_synchroonstartup(void)const;
    void set_synchroonstartup(bool flag);

    bool get_synchroonexit(void)const;
    void set_synchroonexit(bool flag);

    bool getSyncroConsoleDetails(void)const;
    void setSyncroConsoleDetails(bool flag);

    int  getEditorCursorPosition(void)const;
    void setEditorCursorPosition(int n);

    int  getEditorScrollBarPosition(void)const;
    void setEditorScrollBarPosition(int n);

    QString getPasswordMiddleHash(void)const;
    void setPasswordMiddleHash(QString hash);

    bool getPasswordSaveFlag(void)const;
    void setPasswordSaveFlag(bool flag);


    bool getRememberCursorAtHistoryNavigation(void)const;
    void setRememberCursorAtHistoryNavigation(bool flag);

    bool getRememberCursorAtOrdinarySelection(void)const;
    void setRememberCursorAtOrdinarySelection(bool flag);

    int  getUglyQssReplaceHeightForTableView(void)const;
    void setUglyQssReplaceHeightForTableView(int n);


    // Перечень полей, отображаемых в таблице конечных записей
    QStringList getRecordTableShowFields(void)const;
    void setRecordTableShowFields(QStringList fields);

    // Нужно ли отображать горизонтальные заголовки в таблице конечных записей
    bool getRecordTableShowHorizontalHeaders(void)const;
    void setRecordTableShowHorizontalHeaders(bool flag);

    // Whether to show vertical headers in the final table of records   // Нужно ли отображать вертикальные заголовки в таблице конечных записей
    bool getRecordTableShowVerticalHeaders(void)const;
    void setRecordTableShowVerticalHeaders(bool flag);

    // Ширина полей, отображаемых в таблице конечных записей
    QStringList getRecordTableFieldsWidth(void)const;
    void setRecordTableFieldsWidth(QStringList fieldsWidth);

    // Показывать ли сплешскрин при старте программы
    bool getShowSplashScreen(void)const;
    void setShowSplashScreen(bool isShow);

    // Режим интерфейса, возможные значения "desktop" и "mobile"
    QString getInterfaceMode(void)const;
    void setInterfaceMode(QString mode);

    QString getFocusWidget(void)const;
    void setFocusWidget(QString widgetName);

    QStringList getHideEditorTools(void)const;
    void setHideEditorTools(QStringList toolsNames);

    // Флаг, показывающий распахнуты ли все инструметы управления в окне поиска по базе
    bool getFindInBaseExpand(void)const;
    void setFindInBaseExpand(bool state);

    // Разрешено ли использовать собственный формат вывода даты и времени
    bool getEnableCustomDateTimeFormat(void)const;
    void setEnableCustomDateTimeFormat(bool state);

    // Строка собственного формата вывода даты и времени
    QString getCustomDateTimeFormat(void)const;
    void setCustomDateTimeFormat(QString format);

    // Путь на диске по которому пользователь открывал файлы чтобы приаттачить их к записи
    QString getAttachAppendDir(void)const;
    void setAttachAppendDir(QString dir);

    // Путь на диске по которому пользователь сохранял (Сохранить как...) приаттаченные файлы
    QString getAttachSaveAsDir(void)const;
    void setAttachSaveAsDir(QString dir);

    // Разрешать ли для просмотра расшифровывать зашифрованные файлы в директорию корзины MyTetra
    bool getEnableDecryptFileToTrashDirectory(void)const;
    void setEnableDecryptFileToTrashDirectory(bool state);


private:

    QSettings *conf; // было static
    QString get_parameter(QString name)const;

    QStringList removeParameterFromTable(QString removeName, QStringList table);
    QString getParameterTypeFromTable(QString parameterName, QStringList table)const;
    QString getParameterValueFromTable(QString parameterName, QStringList table)const;
    QStringList replaceParameterInTable(QString replaceName, QString replaceType, QString replaceValue, QStringList table);


    void update_version_process(void);

    QStringList get_parameter_table_1(bool withEndSignature = true)const;
    QStringList get_parameter_table_2(bool withEndSignature = true)const;
    QStringList get_parameter_table_3(bool withEndSignature = true)const;
    QStringList get_parameter_table_4(bool withEndSignature = true)const;
    QStringList get_parameter_table_5(bool withEndSignature = true);
    QStringList get_parameter_table_6(bool withEndSignature = true);
    QStringList get_parameter_table_7(bool withEndSignature = true);
    QStringList get_parameter_table_8(bool withEndSignature = true);
    QStringList get_parameter_table_9(bool withEndSignature = true);
    QStringList get_parameter_table_10(bool withEndSignature = true);
    QStringList get_parameter_table_11(bool withEndSignature = true);
    QStringList get_parameter_table_12(bool withEndSignature = true);
    QStringList get_parameter_table_13(bool withEndSignature = true);
    QStringList get_parameter_table_14(bool withEndSignature = true);
    QStringList get_parameter_table_15(bool withEndSignature = true);
    QStringList get_parameter_table_16(bool withEndSignature = true);
    QStringList get_parameter_table_17(bool withEndSignature = true);
    QStringList get_parameter_table_18(bool withEndSignature = true);
    QStringList get_parameter_table_19(bool withEndSignature = true);
    QStringList get_parameter_table_20(bool withEndSignature = true);
    QStringList get_parameter_table_21(bool withEndSignature = true);
    QStringList get_parameter_table_22(bool withEndSignature = true);
    QStringList get_parameter_table_23(bool withEndSignature = true);
    QStringList get_parameter_table_24(bool withEndSignature = true);
    QStringList get_parameter_table_25(bool withEndSignature = true);
    QStringList get_parameter_table_26(bool withEndSignature = true);
    QStringList get_parameter_table_27(bool withEndSignature = true);
    bool is_init_flag;
};

#endif // __APPCONFIG_H__