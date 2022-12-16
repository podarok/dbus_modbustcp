#include <QStringList>
#include <QtDebug>
#include <velib/qt/ve_qitem.hpp>
#include "dbus_service.h"
#include "logging.h"

DBusService::DBusService(VeQItem *serviceRoot, QObject *parent) :
	QObject(parent),
	mServiceRoot(serviceRoot),
	mDeviceInstance(serviceRoot->itemGetOrCreate("/DeviceInstance"))
{
	connect(mDeviceInstance, SIGNAL(valueChanged(VeQItem *, QVariant)),
			this, SLOT(onDeviceInstanceChanged()));
	mDeviceInstance->getValue();
}

VeQItem *DBusService::getItem(const QString &path)
{
	VeQItem *item = mItems.value(path);
	if (item != 0)
		return item;
	item = mServiceRoot->itemGetOrCreate(path);
	mItems[path] = item;
	return item;
}

QString DBusService::getDeviceType(const QString &name)
{
	QStringList elements = name.split(".");
	if (elements.count() < 3)
		return "Unknown";

	return elements[2];
}

void DBusService::onDeviceInstanceChanged()
{
	switch (mDeviceInstance->getState()) {
	case VeQItem::Synchronized:
		qInfo() << QString("[DBusService] Service online: %1 (%2)").
					   arg(mServiceRoot->id()).
					   arg(mDeviceInstance->getValue().toInt());
		break;
	case VeQItem::Offline:
		qInfo() << QString("[DBusService] Service offline: %1 (%2)").
					   arg(mServiceRoot->id()).
					   arg(mDeviceInstance->getValue().toInt());
		break;
	default:
		break;
	}
}

bool DBusService::getConnected()
{
	return mServiceRoot->getState() != VeQItem::Offline;
}
