/*
 * MagicButtonAnimation.cpp
 *
 *  Created on: May 23, 2017
 *      Author: andri
 */

#include "MagicButtonAnimation.h"

MagicButtonAnimation::MagicButtonAnimation(WS2812& ws8212):
ws2812_(ws8212) {

}

MagicButtonAnimation::~MagicButtonAnimation() {
	if (animator_ != NULL) {
		delete animator_;
		animator_ = NULL;
	}
}

void MagicButtonAnimation::start() {
	if (ws2812_.getPixelCount() == 0) {
		return;
	}

	if (animator_ == NULL) {
		animator_ = new WS2812Animator();
	}

	animationPrevStarted_ = true;

	this->ws2812_.clear();
}

void MagicButtonAnimation::stop() {
	animationPrevStarted_ = false;
	animator_->stop();
}

void MagicButtonAnimation::run() {
	if (animator_ != NULL) {
		if (animator_->isRunning()) {
			animator_->run();
			ws2812_.show();
		}
	}
}


// MagicButtonFadeInOutAnimation
MagicButtonFadeInOutAnimation::MagicButtonFadeInOutAnimation(WS2812& ws2812, RgbLedColor_t &color):
		MagicButtonAnimation(ws2812), fadingColor_(color)
{
}

void MagicButtonFadeInOutAnimation::start(uint16_t duration, uint16_t updateInterval) {

	MagicButtonAnimation::start();

	for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
		this->ws2812_.setPixel(pixNum, fadingColor_.red, fadingColor_.green, fadingColor_.blue);
	}

	animator_->start([this](const WS2812Animator::AnimationParam param) {

		//Triangle function
		//y = (A/P) * (P - abs(x % (2*P) - P))
		float x = param.progress * 100;
		float P = 100/2;
		float b = (100/P) * (P - abs(((int)x % (int)(2*P)) - P));

		//float b = sin(param.progress * PI) * 100;
//		ESP_LOGI(TAG, "Progress %f", b);

		for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
			this->ws2812_.setBrightnessPercent(pixNum, b);
		}

		this->ws2812_.show();

	}, [this]() {

		MBANIM_DEBUG_PRINT("Animation DONE");
		if (this->animCompletedCb_ != NULL) {
			this->animCompletedCb_();
		}

	}, duration, updateInterval);
}

void MagicButtonFadeInOutAnimation::stop() {
	MagicButtonAnimation::stop();
}


// MagicButtonArrowAnimation
MagicButtonArrowAnimation::MagicButtonArrowAnimation(WS2812& ws2812, RgbLedColor_t& color):
		MagicButtonAnimation(ws2812), fadingColor_(color) {
}

void MagicButtonArrowAnimation::animateArrow(MagicButtonAnimationArrow_t arrow) {

	arrow = (MagicButtonAnimationArrow_t)((int)arrow - 1);
	//uint8_t ledIndiceses[4][2] = {{4, 5}, {0, 1}, {6, 7}, {2, 3}};
	uint8_t ledIndiceses[4][2] = {{0, 7}, {3, 4}, {1, 2}, {5, 6}};
	uint8_t count = sizeof(ledIndiceses[(int)arrow])/sizeof(uint8_t);

	MagicButtonAnimation::start();

	for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
		this->ws2812_.setPixel(pixNum, fadingColor_.red, fadingColor_.green, fadingColor_.blue);
		this->ws2812_.setBrightness(pixNum, 0);
	}

	animator_->start([this, arrow, ledIndiceses, count](const WS2812Animator::AnimationParam param) {

		//Triangle function
		//y = (A/P) * (P - abs(x % (2*P) - P))
		float x = param.progress * 100;
		float P = 100/2;
		float b = (100/P) * (P - abs(((int)x % (int)(2*P)) - P));

		//float b = sin(param.progress * PI) * 100;
//		ESP_LOGI(TAG, "Progress %f", b);

		for (uint8_t idx = 0; idx < count; idx++) {
			uint8_t pixNum = ledIndiceses[(int)arrow][idx];
			this->ws2812_.setBrightnessPercent(pixNum, b);
		}

		this->ws2812_.show();

	}, [this]() {

		for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
			this->ws2812_.setBrightness(pixNum, 0);
		}
		this->ws2812_.show();

		//MBANIM_DEBUG_PRINT("Animation DONE");
		if (this->animCompletedCb_ != NULL) {
			this->animCompletedCb_();
		}

	}, 600, 2);
}


// MagicButtonFadingAnimation
MagicButtonFadingAnimation::MagicButtonFadingAnimation(WS2812& ws2812,
		RgbLedColor_t& color):
		MagicButtonAnimation(ws2812), fadingColor_(color) {
}

void MagicButtonFadingAnimation::start(uint8_t from, uint8_t to, uint16_t duration) {
	MagicButtonAnimation::start();

	for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
		this->ws2812_.setPixel(pixNum, fadingColor_);
	}

	animator_->start([this, from, to](const WS2812Animator::AnimationParam param) {

		float x = param.progress * 100;
		//float P = 100/2;
		//float b = (100/P) * (P - abs(((int)x % (int)(2*P)) - P));

		uint8_t scaled = (uint8_t)map((uint8_t)x, 0, 100L, from, to);

		for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
			this->ws2812_.setBrightnessPercent(pixNum, scaled);
		}

		this->ws2812_.show();

	}, [this]() {

		MBANIM_DEBUG_PRINT("Fading Animation DONE");
		if (this->animCompletedCb_ != NULL) {
			this->animCompletedCb_();
		}

	}, duration, 10);
}


// MagicButtonGlowAnimation
MagicButtonGlowAnimation::MagicButtonGlowAnimation(WS2812& ws2812,
		RgbLedColor_t& color):
		MagicButtonAnimation(ws2812), glowColor_(color) {
}

void MagicButtonGlowAnimation::start(uint16_t duration) {
	MagicButtonAnimation::start();

	for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
		this->ws2812_.setPixel(pixNum, glowColor_);
	}

	animator_->start([this, duration](const WS2812Animator::AnimationParam param) {

		float s = animator_->getStepFloat(param.elapsed, duration, TWO_PI);
		float k = (-cos(s)+1)/2;

		for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
			this->ws2812_.setPixel(pixNum, glowColor_.scale(k));
		}

		this->ws2812_.show();

	}, [this]() {

		MBANIM_DEBUG_PRINT("Glow Animation DONE");
		if (this->animCompletedCb_ != NULL) {
			this->animCompletedCb_();
		}

	}, duration, 10);
}


// MagicButtonCometAnimation
MagicButtonCometAnimation::MagicButtonCometAnimation(WS2812& ws2812,
		RgbLedPalette_t& cometColorPallete):
		MagicButtonAnimation(ws2812), cometColorPallete_(cometColorPallete){
}

void MagicButtonCometAnimation::start(uint16_t duration, MagicButtonAnimationDir_t dir, uint16_t bouncingCount) {

	MagicButtonAnimation::start();

	for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
		this->ws2812_.setPixel(pixNum, cometColorPallete_.colors[0]);
	}

	animationDirection_ = dir;
	bouncingCount_ = bouncingCount;

	animator_->start([this, duration](const WS2812Animator::AnimationParam param) {

		uint16_t numLeds = this->ws2812_.getPixelCount();
		float l = ((float)numLeds)/3;  // length of the tail
		float t = animator_->getStepFloat(param.elapsed, duration, 2*numLeds - l);
		float tx = animator_->getStepFloat(param.elapsed, duration, cometColorPallete_.numColors);
		RgbLedColor_t c = cometColorPallete_.getPaletteColor(tx);

//		printf("VALS --> l: %f, t: %f, tx: %f\n", l, t, tx);

		for (uint8_t pixNum = 0; pixNum < this->ws2812_.getPixelCount(); pixNum++) {
		//for (uint8_t pixNum = (this->ws2812_.getPixelCount() - 1); pixNum > 0 ; pixNum--) {

			float k = constrain( (((pixNum-t)/l+1.2f))*(((pixNum-t)<0)? 1:0), 0, 1);

			if (animationDirection_ == ANIM_DIR_RIGHT) {
				this->ws2812_.setPixel(pixNum, c.scale(k));
			} else {
				this->ws2812_.setPixel(this->ws2812_.getPixelCount() - pixNum - 1, c.scale(k));
			}
		}

		this->ws2812_.show();

	}, [this, duration]() {

		MBANIM_DEBUG_PRINT("Comet Animation DONE");

		animCompleteCount_++;
		if (animCompleteCount_ > bouncingCount_) {
			if (this->animCompletedCb_ != NULL) {
				this->animCompletedCb_();
			}

			return;
		}

		MagicButtonAnimationDir_t newDir = (animationDirection_ == ANIM_DIR_RIGHT)? ANIM_DIR_LEFT: ANIM_DIR_RIGHT;
		this->start(duration, newDir, bouncingCount_);

	}, duration, 5);
}
