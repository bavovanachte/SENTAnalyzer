#include "SENTSimulationDataGenerator.h"
#include "SENTAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

int fc_data [6] = {27, 17, 22, 14, 20, 12};

SENTSimulationDataGenerator::SENTSimulationDataGenerator()
{
}

SENTSimulationDataGenerator::~SENTSimulationDataGenerator()
{
}

void SENTSimulationDataGenerator::Initialize( U32 simulation_sample_rate, SENTAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mSerialSimulationData.SetChannel( settings->mInputChannel );
	mSerialSimulationData.SetSampleRate( simulation_sample_rate );
	mSerialSimulationData.SetInitialBitState( BIT_HIGH );
}

U32 SENTSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mSerialSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		CreateSerialByte();
	}

	*simulation_channel = &mSerialSimulationData;
	return 1;
}

void SENTSimulationDataGenerator::AddNibble(U16 number_of_ticks, U16 samples_per_tick)
{
	U16 number_of_high_ticks = number_of_ticks - 5;
	mSerialSimulationData.Transition();
	mSerialSimulationData.Advance( samples_per_tick * 5);
	mSerialSimulationData.Transition();
	mSerialSimulationData.Advance( samples_per_tick * number_of_high_ticks);
}

void SENTSimulationDataGenerator::CreateSerialByte()
{
	U32 samples_per_tick = mSimulationSampleRateHz * mSettings->tick_time / 1000000;

	if ( mSettings->pausePulseEnabled )
	{
	    /* First, a normal SENT frame */

		/* Calibration pulse */
		AddNibble(56, samples_per_tick);
		/* Status nibble */
		AddNibble(12, samples_per_tick);
		/* Fast channel nibbles */
		for (U8 counter = 0; counter < mSettings->numberOfDataNibbles; counter++) {
			AddNibble(fc_data[counter], samples_per_tick);
		}
		/* CRC */
		AddNibble(21, samples_per_tick);
		/* Pause pulse */
		AddNibble(100, samples_per_tick);

		/* Then, another valid SENT frame, but with a pause pulse the size of a sync pulse. Muhahahahaa */

		/* Calibration pulse */
		AddNibble(56, samples_per_tick);
		/* Status nibble */
		AddNibble(12, samples_per_tick);
		/* Fast channel nibbles */
		for (U8 counter = 0; counter < mSettings->numberOfDataNibbles; counter++) {
			AddNibble(fc_data[counter], samples_per_tick);
		}
		/* CRC */
		AddNibble(21, samples_per_tick);
		/* Pause pulse */
		AddNibble(56, samples_per_tick);
	}
	else
	{
		/* Then, a valid SENT frame without a pause pulse at the end.
		/* Calibration pulse */
		AddNibble(56, samples_per_tick);
		/* Status nibble */
		AddNibble(12, samples_per_tick);
		/* Fast channel nibbles */
		for (U8 counter = 0; counter < mSettings->numberOfDataNibbles; counter++) {
			AddNibble(fc_data[counter], samples_per_tick);
		}
		/* CRC */
		AddNibble(21, samples_per_tick);
		// /* Pause pulse */
		// AddNibble(100, samples_per_tick);
	}
}


