#pragma once

#include <vector>
#include <limits>

#include <QString>

#include "udp_def.h"

#ifndef _WIN32
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

static const char* KeyIpAddress = "IpAddress";
static const char* KeyPort = "Port";
static const char* KeyIpVersion = "IpVersion";
static const char* KeyData = "Data";


static const char* IpAddressDefault = "";
static const char* DataDefault = "";

const int PortMin = 0;
const int PortDefault = 50000;
const int PortMax = std::numeric_limits<unsigned short>::max();

const ip_version IpVersionDefault = ip_version::v4;
const ip_version IpVersionMin = ip_version::v4;
const ip_version IpVersionMax = ip_version::v6;

struct ApplicationSettings
{
    QString ipAddress;
    int port;
    ip_version ipVersion;
    QString data;

    ApplicationSettings() :
        ipAddress(IpAddressDefault),
        port(PortDefault),
        ipVersion(IpVersionDefault),
        data(DataDefault)
    {

    }
};


static QString IpVersionToString(ip_version version)
{
    switch(version)
    {
    case ip_version::v4:
        return "IPv4";
    case ip_version::v6:
        return "IPv6";
    default:
        return QString::number(static_cast<int>(version));
    }
}

static void ValidateIpVersion(ip_version& version)
{
    if (static_cast<int>(version) < static_cast<int>(IpVersionMin)) version = IpVersionMin;
    if (static_cast<int>(version) > static_cast<int>(IpVersionMax)) version = IpVersionMax;
}

static std::vector<ip_version> GetIpVersions()
{
    std::vector<ip_version> v
    {
        ip_version::v4, ip_version::v6
    };

    return v;
}

static void ValidatePort(int& port)
{
    if (port < PortMin) port = PortMin;
    if (port > PortMax) port = PortMax;
}



