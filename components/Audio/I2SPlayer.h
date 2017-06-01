/*
 * MusicPlayer.h
 *
 *  Created on: 10.04.2017
 *      Author: bernd
 */

#ifndef MAIN_I2SPLAYER_H_
#define MAIN_I2SPLAYER_H_

#include "WAVFile.h"

class I2SPlayer {
public:
	I2SPlayer();
	virtual ~I2SPlayer();

	/*
	 * @brief initializes the I2S subsystem
	 */
	void init();

	/*
	 * @brief parses the wav file and prepares it for play
	 *
	 * @param 	wavdata static array to binary wav-file data
	 * @param 	size	size of wavdata array
	 * @return 	true if wav file could be parsed properly
	 * 			false if wav file format is invalid
	 */
	bool prepareWav(const char* wavFilename);

	/*
	 * @brief 	plays the audio file (can be called repeatedly)
	 * @return 	false if prepareWav was not called or due to Wav sample-rate/channel format mismatch with I2S subsystem
	 */
	bool play();

	/*
	 * @brief 	plays the audio file in a separate thread
	*/
	void playAsync();

	/*
	 * @brief 	sets the player volume in 0..100% (default is 100%)
	 * @param	volume where 100% is loudest and 0% is sound off
	 */
	void setVolume(unsigned char volume);


private:
	WAVFile *wavFile;
	unsigned char volume = 100; // default is max 100% volume

};

#endif /* MAIN_I2SPLAYER_H_ */
