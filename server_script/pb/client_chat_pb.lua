-- Generated By protoc-gen-lua Do not Edit
local protobuf = require "protobuf"
local public_pb = require("public_pb")
local base_pb = require("base_pb")
module('client_chat_pb')


SENDMSG = protobuf.Descriptor();
SENDMSG_CHANNELID_FIELD = protobuf.FieldDescriptor();
SENDMSG_CONTENT_FIELD = protobuf.FieldDescriptor();
SENDMSG_ISAUDIO_FIELD = protobuf.FieldDescriptor();
SENDMSG_AUDIOLEN_FIELD = protobuf.FieldDescriptor();
SENDMSG_AUDIOIDX_FIELD = protobuf.FieldDescriptor();
SENDERINFO = protobuf.Descriptor();
SENDERINFO_SENDERID_FIELD = protobuf.FieldDescriptor();
SENDERINFO_SHAPE_FIELD = protobuf.FieldDescriptor();
SENDERINFO_NAME_FIELD = protobuf.FieldDescriptor();
SENDERINFO_LEVEL_FIELD = protobuf.FieldDescriptor();
SENDERINFO_FLAGLIST_FIELD = protobuf.FieldDescriptor();
SENDERINFO_TEAMID_FIELD = protobuf.FieldDescriptor();
SENDERINFO_GUILDNAME_FIELD = protobuf.FieldDescriptor();
SENDERINFO_SCHOOL_FIELD = protobuf.FieldDescriptor();
FASTCHATINFO = protobuf.Descriptor();
FASTCHATINFO_TEAMID_FIELD = protobuf.FieldDescriptor();
FASTCHATINFO_COUNT_FIELD = protobuf.FieldDescriptor();
FASTCHATINFO_TASK_FIELD = protobuf.FieldDescriptor();
FASTCHATINFO_TARGET_FIELD = protobuf.FieldDescriptor();
AUDIOINFO = protobuf.Descriptor();
AUDIOINFO_AUDIOIDX_FIELD = protobuf.FieldDescriptor();
AUDIOINFO_AUDIOLEN_FIELD = protobuf.FieldDescriptor();
RECEIVEMSG = protobuf.Descriptor();
RECEIVEMSG_CHANNELID_FIELD = protobuf.FieldDescriptor();
RECEIVEMSG_SENDER_FIELD = protobuf.FieldDescriptor();
RECEIVEMSG_CONTENT_FIELD = protobuf.FieldDescriptor();
RECEIVEMSG_ISAUDIO_FIELD = protobuf.FieldDescriptor();
RECEIVEMSG_FASTCHAT_FIELD = protobuf.FieldDescriptor();
RECEIVEMSG_AUDIO_FIELD = protobuf.FieldDescriptor();
RECEIVEMSG_ROLL_FIELD = protobuf.FieldDescriptor();
BANCHANNELMSG = protobuf.Descriptor();
BANCHANNELMSG_CHANNELIDLIST_FIELD = protobuf.FieldDescriptor();

SENDMSG_CHANNELID_FIELD.name = "channelId"
SENDMSG_CHANNELID_FIELD.full_name = ".client_chat.sendMsg.channelId"
SENDMSG_CHANNELID_FIELD.number = 1
SENDMSG_CHANNELID_FIELD.index = 0
SENDMSG_CHANNELID_FIELD.label = 2
SENDMSG_CHANNELID_FIELD.has_default_value = false
SENDMSG_CHANNELID_FIELD.default_value = 0
SENDMSG_CHANNELID_FIELD.type = 5
SENDMSG_CHANNELID_FIELD.cpp_type = 1

SENDMSG_CONTENT_FIELD.name = "content"
SENDMSG_CONTENT_FIELD.full_name = ".client_chat.sendMsg.content"
SENDMSG_CONTENT_FIELD.number = 2
SENDMSG_CONTENT_FIELD.index = 1
SENDMSG_CONTENT_FIELD.label = 1
SENDMSG_CONTENT_FIELD.has_default_value = false
SENDMSG_CONTENT_FIELD.default_value = ""
SENDMSG_CONTENT_FIELD.type = 12
SENDMSG_CONTENT_FIELD.cpp_type = 9

SENDMSG_ISAUDIO_FIELD.name = "isAudio"
SENDMSG_ISAUDIO_FIELD.full_name = ".client_chat.sendMsg.isAudio"
SENDMSG_ISAUDIO_FIELD.number = 3
SENDMSG_ISAUDIO_FIELD.index = 2
SENDMSG_ISAUDIO_FIELD.label = 1
SENDMSG_ISAUDIO_FIELD.has_default_value = false
SENDMSG_ISAUDIO_FIELD.default_value = 0
SENDMSG_ISAUDIO_FIELD.type = 5
SENDMSG_ISAUDIO_FIELD.cpp_type = 1

SENDMSG_AUDIOLEN_FIELD.name = "audioLen"
SENDMSG_AUDIOLEN_FIELD.full_name = ".client_chat.sendMsg.audioLen"
SENDMSG_AUDIOLEN_FIELD.number = 4
SENDMSG_AUDIOLEN_FIELD.index = 3
SENDMSG_AUDIOLEN_FIELD.label = 1
SENDMSG_AUDIOLEN_FIELD.has_default_value = false
SENDMSG_AUDIOLEN_FIELD.default_value = 0
SENDMSG_AUDIOLEN_FIELD.type = 5
SENDMSG_AUDIOLEN_FIELD.cpp_type = 1

SENDMSG_AUDIOIDX_FIELD.name = "audioIdx"
SENDMSG_AUDIOIDX_FIELD.full_name = ".client_chat.sendMsg.audioIdx"
SENDMSG_AUDIOIDX_FIELD.number = 5
SENDMSG_AUDIOIDX_FIELD.index = 4
SENDMSG_AUDIOIDX_FIELD.label = 1
SENDMSG_AUDIOIDX_FIELD.has_default_value = false
SENDMSG_AUDIOIDX_FIELD.default_value = 0
SENDMSG_AUDIOIDX_FIELD.type = 5
SENDMSG_AUDIOIDX_FIELD.cpp_type = 1

SENDMSG.name = "sendMsg"
SENDMSG.full_name = ".client_chat.sendMsg"
SENDMSG.nested_types = {}
SENDMSG.enum_types = {}
SENDMSG.fields = {SENDMSG_CHANNELID_FIELD, SENDMSG_CONTENT_FIELD, SENDMSG_ISAUDIO_FIELD, SENDMSG_AUDIOLEN_FIELD, SENDMSG_AUDIOIDX_FIELD}
SENDMSG.is_extendable = false
SENDMSG.extensions = {}
SENDERINFO_SENDERID_FIELD.name = "senderId"
SENDERINFO_SENDERID_FIELD.full_name = ".client_chat.senderInfo.senderId"
SENDERINFO_SENDERID_FIELD.number = 1
SENDERINFO_SENDERID_FIELD.index = 0
SENDERINFO_SENDERID_FIELD.label = 2
SENDERINFO_SENDERID_FIELD.has_default_value = false
SENDERINFO_SENDERID_FIELD.default_value = 0
SENDERINFO_SENDERID_FIELD.type = 3
SENDERINFO_SENDERID_FIELD.cpp_type = 2

SENDERINFO_SHAPE_FIELD.name = "shape"
SENDERINFO_SHAPE_FIELD.full_name = ".client_chat.senderInfo.shape"
SENDERINFO_SHAPE_FIELD.number = 2
SENDERINFO_SHAPE_FIELD.index = 1
SENDERINFO_SHAPE_FIELD.label = 1
SENDERINFO_SHAPE_FIELD.has_default_value = false
SENDERINFO_SHAPE_FIELD.default_value = 0
SENDERINFO_SHAPE_FIELD.type = 5
SENDERINFO_SHAPE_FIELD.cpp_type = 1

SENDERINFO_NAME_FIELD.name = "name"
SENDERINFO_NAME_FIELD.full_name = ".client_chat.senderInfo.name"
SENDERINFO_NAME_FIELD.number = 3
SENDERINFO_NAME_FIELD.index = 2
SENDERINFO_NAME_FIELD.label = 1
SENDERINFO_NAME_FIELD.has_default_value = false
SENDERINFO_NAME_FIELD.default_value = ""
SENDERINFO_NAME_FIELD.type = 12
SENDERINFO_NAME_FIELD.cpp_type = 9

SENDERINFO_LEVEL_FIELD.name = "level"
SENDERINFO_LEVEL_FIELD.full_name = ".client_chat.senderInfo.level"
SENDERINFO_LEVEL_FIELD.number = 4
SENDERINFO_LEVEL_FIELD.index = 3
SENDERINFO_LEVEL_FIELD.label = 1
SENDERINFO_LEVEL_FIELD.has_default_value = false
SENDERINFO_LEVEL_FIELD.default_value = 0
SENDERINFO_LEVEL_FIELD.type = 5
SENDERINFO_LEVEL_FIELD.cpp_type = 1

SENDERINFO_FLAGLIST_FIELD.name = "flagList"
SENDERINFO_FLAGLIST_FIELD.full_name = ".client_chat.senderInfo.flagList"
SENDERINFO_FLAGLIST_FIELD.number = 5
SENDERINFO_FLAGLIST_FIELD.index = 4
SENDERINFO_FLAGLIST_FIELD.label = 3
SENDERINFO_FLAGLIST_FIELD.has_default_value = false
SENDERINFO_FLAGLIST_FIELD.default_value = {}
SENDERINFO_FLAGLIST_FIELD.type = 5
SENDERINFO_FLAGLIST_FIELD.cpp_type = 1

SENDERINFO_TEAMID_FIELD.name = "teamId"
SENDERINFO_TEAMID_FIELD.full_name = ".client_chat.senderInfo.teamId"
SENDERINFO_TEAMID_FIELD.number = 6
SENDERINFO_TEAMID_FIELD.index = 5
SENDERINFO_TEAMID_FIELD.label = 1
SENDERINFO_TEAMID_FIELD.has_default_value = false
SENDERINFO_TEAMID_FIELD.default_value = 0
SENDERINFO_TEAMID_FIELD.type = 5
SENDERINFO_TEAMID_FIELD.cpp_type = 1

SENDERINFO_GUILDNAME_FIELD.name = "guildName"
SENDERINFO_GUILDNAME_FIELD.full_name = ".client_chat.senderInfo.guildName"
SENDERINFO_GUILDNAME_FIELD.number = 7
SENDERINFO_GUILDNAME_FIELD.index = 6
SENDERINFO_GUILDNAME_FIELD.label = 1
SENDERINFO_GUILDNAME_FIELD.has_default_value = false
SENDERINFO_GUILDNAME_FIELD.default_value = ""
SENDERINFO_GUILDNAME_FIELD.type = 12
SENDERINFO_GUILDNAME_FIELD.cpp_type = 9

SENDERINFO_SCHOOL_FIELD.name = "school"
SENDERINFO_SCHOOL_FIELD.full_name = ".client_chat.senderInfo.school"
SENDERINFO_SCHOOL_FIELD.number = 8
SENDERINFO_SCHOOL_FIELD.index = 7
SENDERINFO_SCHOOL_FIELD.label = 1
SENDERINFO_SCHOOL_FIELD.has_default_value = false
SENDERINFO_SCHOOL_FIELD.default_value = 0
SENDERINFO_SCHOOL_FIELD.type = 5
SENDERINFO_SCHOOL_FIELD.cpp_type = 1

SENDERINFO.name = "senderInfo"
SENDERINFO.full_name = ".client_chat.senderInfo"
SENDERINFO.nested_types = {}
SENDERINFO.enum_types = {}
SENDERINFO.fields = {SENDERINFO_SENDERID_FIELD, SENDERINFO_SHAPE_FIELD, SENDERINFO_NAME_FIELD, SENDERINFO_LEVEL_FIELD, SENDERINFO_FLAGLIST_FIELD, SENDERINFO_TEAMID_FIELD, SENDERINFO_GUILDNAME_FIELD, SENDERINFO_SCHOOL_FIELD}
SENDERINFO.is_extendable = false
SENDERINFO.extensions = {}
FASTCHATINFO_TEAMID_FIELD.name = "teamId"
FASTCHATINFO_TEAMID_FIELD.full_name = ".client_chat.fastChatInfo.teamId"
FASTCHATINFO_TEAMID_FIELD.number = 1
FASTCHATINFO_TEAMID_FIELD.index = 0
FASTCHATINFO_TEAMID_FIELD.label = 2
FASTCHATINFO_TEAMID_FIELD.has_default_value = false
FASTCHATINFO_TEAMID_FIELD.default_value = 0
FASTCHATINFO_TEAMID_FIELD.type = 5
FASTCHATINFO_TEAMID_FIELD.cpp_type = 1

FASTCHATINFO_COUNT_FIELD.name = "count"
FASTCHATINFO_COUNT_FIELD.full_name = ".client_chat.fastChatInfo.count"
FASTCHATINFO_COUNT_FIELD.number = 2
FASTCHATINFO_COUNT_FIELD.index = 1
FASTCHATINFO_COUNT_FIELD.label = 1
FASTCHATINFO_COUNT_FIELD.has_default_value = false
FASTCHATINFO_COUNT_FIELD.default_value = 0
FASTCHATINFO_COUNT_FIELD.type = 5
FASTCHATINFO_COUNT_FIELD.cpp_type = 1

FASTCHATINFO_TASK_FIELD.name = "task"
FASTCHATINFO_TASK_FIELD.full_name = ".client_chat.fastChatInfo.task"
FASTCHATINFO_TASK_FIELD.number = 3
FASTCHATINFO_TASK_FIELD.index = 2
FASTCHATINFO_TASK_FIELD.label = 1
FASTCHATINFO_TASK_FIELD.has_default_value = false
FASTCHATINFO_TASK_FIELD.default_value = 0
FASTCHATINFO_TASK_FIELD.type = 5
FASTCHATINFO_TASK_FIELD.cpp_type = 1

FASTCHATINFO_TARGET_FIELD.name = "target"
FASTCHATINFO_TARGET_FIELD.full_name = ".client_chat.fastChatInfo.target"
FASTCHATINFO_TARGET_FIELD.number = 4
FASTCHATINFO_TARGET_FIELD.index = 3
FASTCHATINFO_TARGET_FIELD.label = 3
FASTCHATINFO_TARGET_FIELD.has_default_value = false
FASTCHATINFO_TARGET_FIELD.default_value = {}
FASTCHATINFO_TARGET_FIELD.type = 5
FASTCHATINFO_TARGET_FIELD.cpp_type = 1

FASTCHATINFO.name = "fastChatInfo"
FASTCHATINFO.full_name = ".client_chat.fastChatInfo"
FASTCHATINFO.nested_types = {}
FASTCHATINFO.enum_types = {}
FASTCHATINFO.fields = {FASTCHATINFO_TEAMID_FIELD, FASTCHATINFO_COUNT_FIELD, FASTCHATINFO_TASK_FIELD, FASTCHATINFO_TARGET_FIELD}
FASTCHATINFO.is_extendable = false
FASTCHATINFO.extensions = {}
AUDIOINFO_AUDIOIDX_FIELD.name = "audioIdx"
AUDIOINFO_AUDIOIDX_FIELD.full_name = ".client_chat.audioInfo.audioIdx"
AUDIOINFO_AUDIOIDX_FIELD.number = 1
AUDIOINFO_AUDIOIDX_FIELD.index = 0
AUDIOINFO_AUDIOIDX_FIELD.label = 2
AUDIOINFO_AUDIOIDX_FIELD.has_default_value = false
AUDIOINFO_AUDIOIDX_FIELD.default_value = 0
AUDIOINFO_AUDIOIDX_FIELD.type = 5
AUDIOINFO_AUDIOIDX_FIELD.cpp_type = 1

AUDIOINFO_AUDIOLEN_FIELD.name = "audioLen"
AUDIOINFO_AUDIOLEN_FIELD.full_name = ".client_chat.audioInfo.audioLen"
AUDIOINFO_AUDIOLEN_FIELD.number = 2
AUDIOINFO_AUDIOLEN_FIELD.index = 1
AUDIOINFO_AUDIOLEN_FIELD.label = 1
AUDIOINFO_AUDIOLEN_FIELD.has_default_value = false
AUDIOINFO_AUDIOLEN_FIELD.default_value = 0
AUDIOINFO_AUDIOLEN_FIELD.type = 5
AUDIOINFO_AUDIOLEN_FIELD.cpp_type = 1

AUDIOINFO.name = "audioInfo"
AUDIOINFO.full_name = ".client_chat.audioInfo"
AUDIOINFO.nested_types = {}
AUDIOINFO.enum_types = {}
AUDIOINFO.fields = {AUDIOINFO_AUDIOIDX_FIELD, AUDIOINFO_AUDIOLEN_FIELD}
AUDIOINFO.is_extendable = false
AUDIOINFO.extensions = {}
RECEIVEMSG_CHANNELID_FIELD.name = "channelId"
RECEIVEMSG_CHANNELID_FIELD.full_name = ".client_chat.receiveMsg.channelId"
RECEIVEMSG_CHANNELID_FIELD.number = 1
RECEIVEMSG_CHANNELID_FIELD.index = 0
RECEIVEMSG_CHANNELID_FIELD.label = 2
RECEIVEMSG_CHANNELID_FIELD.has_default_value = false
RECEIVEMSG_CHANNELID_FIELD.default_value = 0
RECEIVEMSG_CHANNELID_FIELD.type = 5
RECEIVEMSG_CHANNELID_FIELD.cpp_type = 1

RECEIVEMSG_SENDER_FIELD.name = "sender"
RECEIVEMSG_SENDER_FIELD.full_name = ".client_chat.receiveMsg.sender"
RECEIVEMSG_SENDER_FIELD.number = 2
RECEIVEMSG_SENDER_FIELD.index = 1
RECEIVEMSG_SENDER_FIELD.label = 1
RECEIVEMSG_SENDER_FIELD.has_default_value = false
RECEIVEMSG_SENDER_FIELD.default_value = nil
RECEIVEMSG_SENDER_FIELD.message_type = SENDERINFO
RECEIVEMSG_SENDER_FIELD.type = 11
RECEIVEMSG_SENDER_FIELD.cpp_type = 10

RECEIVEMSG_CONTENT_FIELD.name = "content"
RECEIVEMSG_CONTENT_FIELD.full_name = ".client_chat.receiveMsg.content"
RECEIVEMSG_CONTENT_FIELD.number = 3
RECEIVEMSG_CONTENT_FIELD.index = 2
RECEIVEMSG_CONTENT_FIELD.label = 1
RECEIVEMSG_CONTENT_FIELD.has_default_value = false
RECEIVEMSG_CONTENT_FIELD.default_value = ""
RECEIVEMSG_CONTENT_FIELD.type = 12
RECEIVEMSG_CONTENT_FIELD.cpp_type = 9

RECEIVEMSG_ISAUDIO_FIELD.name = "isAudio"
RECEIVEMSG_ISAUDIO_FIELD.full_name = ".client_chat.receiveMsg.isAudio"
RECEIVEMSG_ISAUDIO_FIELD.number = 4
RECEIVEMSG_ISAUDIO_FIELD.index = 3
RECEIVEMSG_ISAUDIO_FIELD.label = 1
RECEIVEMSG_ISAUDIO_FIELD.has_default_value = false
RECEIVEMSG_ISAUDIO_FIELD.default_value = 0
RECEIVEMSG_ISAUDIO_FIELD.type = 5
RECEIVEMSG_ISAUDIO_FIELD.cpp_type = 1

RECEIVEMSG_FASTCHAT_FIELD.name = "fastChat"
RECEIVEMSG_FASTCHAT_FIELD.full_name = ".client_chat.receiveMsg.fastChat"
RECEIVEMSG_FASTCHAT_FIELD.number = 5
RECEIVEMSG_FASTCHAT_FIELD.index = 4
RECEIVEMSG_FASTCHAT_FIELD.label = 1
RECEIVEMSG_FASTCHAT_FIELD.has_default_value = false
RECEIVEMSG_FASTCHAT_FIELD.default_value = nil
RECEIVEMSG_FASTCHAT_FIELD.message_type = FASTCHATINFO
RECEIVEMSG_FASTCHAT_FIELD.type = 11
RECEIVEMSG_FASTCHAT_FIELD.cpp_type = 10

RECEIVEMSG_AUDIO_FIELD.name = "audio"
RECEIVEMSG_AUDIO_FIELD.full_name = ".client_chat.receiveMsg.audio"
RECEIVEMSG_AUDIO_FIELD.number = 6
RECEIVEMSG_AUDIO_FIELD.index = 5
RECEIVEMSG_AUDIO_FIELD.label = 1
RECEIVEMSG_AUDIO_FIELD.has_default_value = false
RECEIVEMSG_AUDIO_FIELD.default_value = nil
RECEIVEMSG_AUDIO_FIELD.message_type = AUDIOINFO
RECEIVEMSG_AUDIO_FIELD.type = 11
RECEIVEMSG_AUDIO_FIELD.cpp_type = 10

RECEIVEMSG_ROLL_FIELD.name = "roll"
RECEIVEMSG_ROLL_FIELD.full_name = ".client_chat.receiveMsg.roll"
RECEIVEMSG_ROLL_FIELD.number = 7
RECEIVEMSG_ROLL_FIELD.index = 6
RECEIVEMSG_ROLL_FIELD.label = 1
RECEIVEMSG_ROLL_FIELD.has_default_value = false
RECEIVEMSG_ROLL_FIELD.default_value = 0
RECEIVEMSG_ROLL_FIELD.type = 5
RECEIVEMSG_ROLL_FIELD.cpp_type = 1

RECEIVEMSG.name = "receiveMsg"
RECEIVEMSG.full_name = ".client_chat.receiveMsg"
RECEIVEMSG.nested_types = {}
RECEIVEMSG.enum_types = {}
RECEIVEMSG.fields = {RECEIVEMSG_CHANNELID_FIELD, RECEIVEMSG_SENDER_FIELD, RECEIVEMSG_CONTENT_FIELD, RECEIVEMSG_ISAUDIO_FIELD, RECEIVEMSG_FASTCHAT_FIELD, RECEIVEMSG_AUDIO_FIELD, RECEIVEMSG_ROLL_FIELD}
RECEIVEMSG.is_extendable = false
RECEIVEMSG.extensions = {}
BANCHANNELMSG_CHANNELIDLIST_FIELD.name = "channelIdList"
BANCHANNELMSG_CHANNELIDLIST_FIELD.full_name = ".client_chat.banChannelMsg.channelIdList"
BANCHANNELMSG_CHANNELIDLIST_FIELD.number = 1
BANCHANNELMSG_CHANNELIDLIST_FIELD.index = 0
BANCHANNELMSG_CHANNELIDLIST_FIELD.label = 3
BANCHANNELMSG_CHANNELIDLIST_FIELD.has_default_value = false
BANCHANNELMSG_CHANNELIDLIST_FIELD.default_value = {}
BANCHANNELMSG_CHANNELIDLIST_FIELD.type = 5
BANCHANNELMSG_CHANNELIDLIST_FIELD.cpp_type = 1

BANCHANNELMSG.name = "banChannelMsg"
BANCHANNELMSG.full_name = ".client_chat.banChannelMsg"
BANCHANNELMSG.nested_types = {}
BANCHANNELMSG.enum_types = {}
BANCHANNELMSG.fields = {BANCHANNELMSG_CHANNELIDLIST_FIELD}
BANCHANNELMSG.is_extendable = false
BANCHANNELMSG.extensions = {}

audioInfo = protobuf.Message(AUDIOINFO)
banChannelMsg = protobuf.Message(BANCHANNELMSG)
fastChatInfo = protobuf.Message(FASTCHATINFO)
receiveMsg = protobuf.Message(RECEIVEMSG)
sendMsg = protobuf.Message(SENDMSG)
senderInfo = protobuf.Message(SENDERINFO)
