//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2021  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"
#include "config.h"
#include <circle/util.h>
#include <assert.h>

#if WRITE_FORMAT == 0
	#define FORMAT		SoundFormatUnsigned8
	#define TYPE		u8
	#define TYPE_SIZE	sizeof (u8)
	#define FACTOR		((1 << 7)-1)
	#define NULL_LEVEL	(1 << 7)
#elif WRITE_FORMAT == 1
	#define FORMAT		SoundFormatSigned16
	#define TYPE		s16
	#define TYPE_SIZE	sizeof (s16)
	#define FACTOR		((1 << 15)-1)
	#define NULL_LEVEL	0
#elif WRITE_FORMAT == 2
	#define FORMAT		SoundFormatSigned24
	#define TYPE		s32
	#define TYPE_SIZE	(sizeof (u8)*3)
	#define FACTOR		((1 << 23)-1)
	#define NULL_LEVEL	0
#elif WRITE_FORMAT == 3
	#define FORMAT		SoundFormatSigned24_32
	#define TYPE		s32
	#define TYPE_SIZE	(sizeof (s32))
	#define FACTOR		((1 << 23)-1)
	#define NULL_LEVEL	0
#endif

static const char FromKernel[] = "kernel";

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_SoundIn (&m_Interrupt, SAMPLE_RATE, CHUNK_SIZE, TRUE, 0, 0,
		   CI2SSoundBaseDevice::DeviceModeRXOnly),
	m_SoundOut (&m_Interrupt, SAMPLE_RATE, CHUNK_SIZE)
{
	m_ActLED.Blink (5);	// show we are alive
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}

	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (pTarget);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	// configure sound device
	if (!m_SoundIn.AllocateReadQueue (QUEUE_SIZE_MSECS))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot allocate input sound queue");
	}

	m_SoundIn.SetReadFormat (FORMAT, WRITE_CHANNELS);

	if (!m_SoundOut.AllocateQueue (QUEUE_SIZE_MSECS))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot allocate output sound queue");
	}

	m_SoundOut.SetWriteFormat (FORMAT, WRITE_CHANNELS);

	// start sound devices
	if (!m_SoundIn.Start ())
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot start input sound device");
	}

	if (!m_SoundOut.Start ())
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot start output sound device");
	}

	// copy sound data
	for (unsigned nCount = 0; 1; nCount++)
	{
		u8 Buffer[TYPE_SIZE*WRITE_CHANNELS*1000];
		int nBytes = m_SoundIn.Read (Buffer, sizeof Buffer);
		if (nBytes > 0)
		{
			int nResult = m_SoundOut.Write (Buffer, nBytes);
			if (nResult != nBytes)
			{
				m_Logger.Write (FromKernel, LogWarning, "Sound data dropped");
			}
		}

		m_Screen.Rotor (0, nCount);
	}

	return ShutdownHalt;
}
