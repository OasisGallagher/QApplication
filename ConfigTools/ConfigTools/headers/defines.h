#pragma once

#define VIDEO_PATH					"Videos"
#define PICTURE_PATH				"CoverPictures"

/**
 * Setting.
 */
#define DATA_PATH					""
#define SETTING_PATH				DATA_PATH "setting.xml"
#define SETTING_ROOT				"setting"
#define SETTING_CATEGORIES			"categories"
#define SETTING_CATEGORY			"category"
#define SETTING_VIDEO_POSTFIX		"videoPostfix"
#define SETTING_PICTURE_POSTFIX		"picturePostfix"

/**
 * Config common.
 */
#define TAG_ID						"id"

/**
 * Video config.
 */
#define VF_PATH						DATA_PATH "videos.json"
#define VF_TAG_ROOT					"VideoList"
#define VF_TAG_NAME					"name"
#define VF_TAG_VIDEO				"video"
#define VF_TAG_PICTURE				"picture"
#define VF_TAG_CATEGORY				"category"

/**
 * Client config.
 */
#define CF_PATH						DATA_PATH "clients.json"
#define CF_TAG_ROOT					"ClientList"
#define CF_TAG_IP					"ip"
