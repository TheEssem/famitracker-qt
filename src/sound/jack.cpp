#include <jack/jack.h>
#include <cmath>
#include <malloc.h>
#include <stdio.h>
#include "jack.hpp"

core_api_SoundSink * sound_create()
{
	return new JackSound;
}

typedef jack_default_audio_sample_t sample_t;

struct jacksound_info_t
{
	jack_client_t * client;

	jack_port_t * out;
};

static int process(jack_nframes_t frames, void *arg)
{
	// this is the moment when the last period should be playing
	// notify time dispatcher to tick for the duration of this period

	jacksound_info_t *handle = (jacksound_info_t*)arg;

	void *buf = jack_port_get_buffer(handle->out, frames);

	return 0;
}

JackSound::JackSound()
{
	m_handle = new jacksound_info_t;
	m_handle->client = NULL;
}
JackSound::~JackSound()
{
	JackSound::close();
	delete m_handle;
}

void JackSound::initialize(unsigned int sampleRate, unsigned int channels, unsigned int latency_ms)
{
	if (m_handle->client != NULL)
	{
		JackSound::close();
	}

	m_handle->client = jack_client_open("FamiTracker", JackNullOption, NULL);
	if (!m_handle->client)
	{
		fprintf (stderr, "jack server not running?\n");
		return;
	}

	sampleRate = jack_get_sample_rate(m_handle->client);

	jack_set_process_callback(m_handle->client, process, m_handle);

	m_handle->out = jack_port_register(m_handle->client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if (jack_activate(m_handle->client))
	{
		fprintf(stderr, "cannot activate client");
		return;
	}

	const char **ports;

	ports = jack_get_ports(m_handle->client, NULL, NULL, JackPortIsPhysical|JackPortIsInput);

	if (ports == NULL)
	{
		fprintf(stderr, "Cannot find any physical playback ports\n");
		return;
	}

	if (jack_connect(m_handle->client, jack_port_name(m_handle->out), ports[0]) != 0)
	{
		fprintf(stderr, "Cannot connect output ports\n");
	}
	if (jack_connect(m_handle->client, jack_port_name(m_handle->out), ports[1]) != 0)
	{
		fprintf(stderr, "Cannot connect output ports\n");
	}

	free(ports);
}
void JackSound::close()
{
	jack_client_close(m_handle->client);
	m_handle->client = NULL;
}
void JackSound::flushBuffer(core::s16 *Buffer, core::u32 Size)
{

}
void JackSound::flush()
{
}
int JackSound::sampleRate() const
{
	return jack_get_sample_rate(m_handle->client);
}
