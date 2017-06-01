/*
 * MagicButtonAnimation.h
 *
 *  Created on: May 23, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_MAGICBUTTON_SRC_MAGICBUTTONANIMATION_H_
#define COMPONENTS_MAGICBUTTON_SRC_MAGICBUTTONANIMATION_H_

#include "WS2812.h"
#include "WS2812Animator.h"
#include "esp_log.h"

#define DEBUG_MBANIM 	1

#if DEBUG_MBANIM
#define MBANIM_DEBUG_PRINT(...)   	ESP_LOGI("MB_ANIM", __VA_ARGS__); //ESP_LOGD("GESTMGR", __VA_ARGS__);
#else
#define MBANIM_DEBUG_PRINT(...)
#endif

class MagicButtonAnimation {
public:
	MagicButtonAnimation(WS2812 &ws8212);
	virtual ~MagicButtonAnimation();

	void start();
	void stop();
	void run();
	void onAnimationCompleted(WS2812Animator::AnimationFinishedCallback cb) {
		animCompletedCb_ = cb;
	}

protected:
	WS2812 &ws2812_;
	WS2812Animator *animator_ = NULL;

	WS2812Animator::AnimationFinishedCallback animCompletedCb_ = NULL;
	boolean animationPrevStarted_ = false;
};

class MagicButtonFadeInOutAnimation: public MagicButtonAnimation {
public:
	MagicButtonFadeInOutAnimation(WS2812 &ws2812, RgbLedColor_t &color);

	void start(uint16_t duration = 0, uint16_t updateInterval = 10);
	void stop();

protected:
	RgbLedColor_t& fadingColor_;
};

enum MagicButtonAnimationArrow_t {
	ARROW_LEFT = 1,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN
};

enum MagicButtonAnimationDir_t {
	ANIM_DIR_RIGHT = 0,
	ANIM_DIR_LEFT
};

class MagicButtonArrowAnimation: public MagicButtonAnimation {
public:
	MagicButtonArrowAnimation(WS2812 &ws2812, RgbLedColor_t &color);
	void animateArrow(MagicButtonAnimationArrow_t arrow);
protected:
	RgbLedColor_t& fadingColor_;
};

class MagicButtonFadingAnimation: public MagicButtonAnimation {
public:
	MagicButtonFadingAnimation(WS2812 &ws2812, RgbLedColor_t &color);

	void start(uint8_t from = 0, uint8_t to = 100, uint16_t duration = 2000);

protected:
	RgbLedColor_t& fadingColor_;
};


class MagicButtonGlowAnimation: public MagicButtonAnimation {
public:
	MagicButtonGlowAnimation(WS2812 &ws2812, RgbLedColor_t &color);

	void start(uint16_t duration = 2000);

protected:
	RgbLedColor_t& glowColor_;
};


class MagicButtonCometAnimation: public MagicButtonAnimation {
public:
	MagicButtonCometAnimation(WS2812 &ws2812, RgbLedPalette_t& cometColorPallete);

	void start(uint16_t duration = 2000, MagicButtonAnimationDir_t dir = ANIM_DIR_RIGHT, uint16_t bouncingCount = 0);

protected:
	RgbLedPalette_t& cometColorPallete_;
	MagicButtonAnimationDir_t animationDirection_ = ANIM_DIR_RIGHT;

	uint16_t bouncingCount_ = 0, animCompleteCount_ = 0;
};
#endif /* COMPONENTS_MAGICBUTTON_SRC_MAGICBUTTONANIMATION_H_ */
