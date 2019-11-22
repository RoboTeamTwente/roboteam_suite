
#include "Channels.h"

namespace proto {

const std::map<ChannelType, Channel> CHANNELS = {
    {GEOMETRY_CHANNEL, {"geometry", "tcp://192.168.1.33:5556"}},
    {REFEREE_CHANNEL, {"referee", "tcp://192.168.1.33:5557"}},
    {WORLD_CHANNEL, {"world", "tcp://192.168.1.33:5558"}},
    {ROBOT_COMMANDS_PRIMARY_CHANNEL, {"commands_primary", "tcp://192.168.1.33:5559"}},
    {ROBOT_COMMANDS_SECONDARY_CHANNEL, {"commands_secondary", "tcp://192.168.1.33:5560"}},
    {FEEDBACK_PRIMARY_CHANNEL, {"feedback_primary", "tcp://192.168.1.33:5561"}},
    {FEEDBACK_SECONDARY_CHANNEL, {"feedback_yellow", "tcp://192.168.1.33:5562"}},
    {SETTINGS_PRIMARY_CHANNEL, {"settings_primary", "tcp://192.168.1.33:5563"}},
    {SETTINGS_SECONDARY_CHANNEL, {"settings_secondary", "tcp://192.168.1.33:5564"}}
};

} // proto