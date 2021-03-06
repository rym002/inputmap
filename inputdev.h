/*

Copyright 2017, Rodrigo Rivas Costa <rodrigorivascosta@gmail.com>

This file is part of inputmap.

inputmap is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

inputmap is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with inputmap.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef INPUTDEV_H_INCLUDED
#define INPUTDEV_H_INCLUDED

#include <memory>
#include <linux/input.h>
#include "steam/fd.h"
#include "inifile.h"

int bus_id(const char *bus_name);

struct InputStatus
{
    int abs[ABS_CNT];
    int rel[REL_CNT];
    bool key[KEY_CNT];
    input_absinfo absinfo[ABS_CNT];

    InputStatus()
    {
        reset();
    }
    void reset()
    {
        memset(abs, 0, sizeof(abs));
        memset(rel, 0, sizeof(rel));
        memset(key, 0, sizeof(key));
    }
};

struct ValueId
{
    int type;
    int code;

    ValueId()
        :type(0), code(0)
    {}
    ValueId(int t, int c)
        :type(t), code(c)
    {}
};

class InputDevice;

enum class PollResult
{
    None,
    Error,
    Sync,
};

struct IPollable
{
    virtual ~IPollable() {}
    virtual int fd() =0;
    virtual PollResult on_poll(int event) =0;
};

typedef float value_t;

class InputDevice : public std::enable_shared_from_this<InputDevice>,
                    public IPollable
{
public:
    const std::string &name() const noexcept
    { return m_name; }

    virtual ValueId parse_value(const std::string &name) =0;
    virtual value_t get_value(const ValueId &id) =0;
    virtual int ff_upload(const ff_effect &eff) =0;
    virtual int ff_erase(int id) =0;
    virtual void ff_run(int eff, bool on) =0;
    virtual void flush() =0;

protected:
    InputDevice(const IniSection &ini);
private:
    std::string m_name;
};

std::shared_ptr<InputDevice> InputDeviceEventCreate(const IniSection &ini, FD fd);

class InputDeviceEvent : public InputDevice
{
public:
    explicit InputDeviceEvent(const IniSection &ini, FD fd);

    virtual int fd()
    { return m_fd.get(); }
    virtual ValueId parse_value(const std::string &name);
    virtual PollResult on_poll(int event);
    virtual value_t get_value(const ValueId &id);
    virtual int ff_upload(const ff_effect &eff);
    virtual int ff_erase(int id);
    virtual void ff_run(int eff, bool on);
    virtual void flush();
private:
    FD m_fd;
    input_event m_evs[128];
    int m_num_evs;
    InputStatus m_status;

    void on_input(input_event &ev);
};

#endif /* INPUTDEV_H_INCLUDED */
