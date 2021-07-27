#pragma once
#ifdef __linux__ 
#include "Linux/control.h"
#include "Linux/connect.h"
#elif _WIN32
#include "Windows/winConnect.h"
#include "Windows/control.h"
#endif

/**
 * IPC Command messages opcodes.
 * A list of possible operations possible by the IPC.
 * Each one of them is what we call an "opcode" and is the first
 * byte sent by the IPC to differentiate between commands.
 */
enum IPCCommand : unsigned char
{
	MsgRead8 = 0,           /**< Read 8 bit value to memory. */
	MsgRead16 = 1,          /**< Read 16 bit value to memory. */
	MsgRead32 = 2,          /**< Read 32 bit value to memory. */
	MsgRead64 = 3,          /**< Read 64 bit value to memory. */
	MsgWrite8 = 4,          /**< Write 8 bit value to memory. */
	MsgWrite16 = 5,         /**< Write 16 bit value to memory. */
	MsgWrite32 = 6,         /**< Write 32 bit value to memory. */
	MsgWrite64 = 7,         /**< Write 64 bit value to memory. */
	MsgVersion = 8,         /**< Returns PCSX2 version. */
	MsgSaveState = 9,       /**< Saves a savestate. */
	MsgLoadState = 0xA,     /**< Loads a savestate. */
	MsgTitle = 0xB,         /**< Returns the game title. */
	MsgID = 0xC,            /**< Returns the game ID. */
	MsgUUID = 0xD,          /**< Returns the game UUID. */
	MsgGameVersion = 0xE,   /**< Returns the game verion. */
	MsgStatus = 0xF,        /**< Returns the emulator status. */
	MsgUnimplemented = 0xFF /**< Unimplemented IPC message. */
};

/**
 * Emulator status enum.
 * A list of possible emulator statuses.
 */
enum EmuStatus : uint32_t
{
	Running = 0, /**< Game is running */
	Paused = 1,  /**< Game is paused */
	Shutdown = 2 /**< Game is shutdown */
};

/**
 * IPC message buffer.
 * A list of all needed fields to store an IPC message.
 */
struct IPCBuffer
{
	int size;     /**< Size of the buffer. */
	char* buffer; /**< Buffer. */
};

/**
 * IPC result codes.
 * A list of possible result codes the IPC can send back.
 * Each one of them is what we call an "opcode" or "tag" and is the
 * first byte sent by the IPC to differentiate between results.
 */
enum IPCResult : unsigned char
{
	IPC_OK = 0,     /**< IPC command successfully completed. */
	IPC_FAIL = 0xFF /**< IPC command failed to complete. */
};

class IPC
{
private:
	Connect connect;
//	std::vector<uint32_t> mem;
	char* buffer;
public:
	bool open(fs::path socketPath, uint8_t slot);

	// Return Address type at pos
	template<typename T>
	T read(int loc, IPCCommand com)
	{
		buffer = connect.read();
		switch ((IPCCommand)buffer[strlen(buffer) - 1])
		{
		case IPCCommand::MsgRead8:
			return read8();
			break;
		case IPCCommand::MsgRead16:
			return read16();
			break;
		case IPCCommand::MsgRead32:
			return read32();
			break;
		}
	}

	uint8_t read8();
	uint16_t read16();
	uint32_t read32();
	uint64_t read64();

	std::string gameTitle();
};