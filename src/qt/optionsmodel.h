#ifndef OPTIONSMODEL_H
#define OPTIONSMODEL_H

#include <QAbstractListModel>

/** Interface from Qt to configuration data structure for Trollocoin client.
   To Qt, the options are presented as a list with the different options
   laid out vertically.
   This can be changed to a tree once the settings become sufficiently
   complex.
 */
class OptionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit OptionsModel(QObject *parent = 0);

    enum OptionID {
        StartAtStartup, // bool
        MinimizeToTray, // bool
        MapPortUPnP, // bool
        MinimizeOnClose, // bool
        ProxyUse, // bool
        ProxySocksVersion, // int
        ProxyIP, // QString
        ProxyPort, // QString
        Fee, // qint64
        DisplayUnit, // BitcoinUnits::Unit
        DisplayAddresses, // bool
        DetachDatabases, // bool
        CoinControlFeatures, // bool
        Language, // QString
        OptionIDRowCount,
    };

    void Init();

    /* Migrate settings from wallet.dat after app initialization */
    bool Upgrade(); /* returns true if settings upgraded */

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    /* Explicit getters */
    qint64 getTransactionFee();
    bool getMinimizeToTray() { return fMinimizeToTray; }
    bool getMinimizeOnClose() { return fMinimizeOnClose; }
    int getDisplayUnit() { return nDisplayUnit; }
    bool getDisplayAddresses() { return bDisplayAddresses; }
    bool getCoinControlFeatures();
    QString getLanguage() { return language; }
private:
    int nDisplayUnit;
    bool bDisplayAddresses;
    bool fMinimizeToTray;
    bool fMinimizeOnClose;
    bool fCoinControlFeatures;
    QString language;
signals:
    void displayUnitChanged(int unit);
    void transactionFeeChanged(qint64);
    void coinControlFeaturesChanged(bool);
};

#endif // OPTIONSMODEL_H
