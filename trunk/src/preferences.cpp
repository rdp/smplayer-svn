/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2012 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "preferences.h"
#include "global.h"
#include "paths.h"
#include "mediasettings.h"
#include "recents.h"
#include "urlhistory.h"
#include "filters.h"

#include <QSettings>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include <QLocale>

#if QT_VERSION >= 0x040400
#include <QDesktopServices>
#endif

#ifdef YOUTUBE_SUPPORT
#include "retrieveyoutubeurl.h"
#endif

using namespace Global;

Preferences::Preferences() {
	history_recents = new Recents;
	history_urls = new URLHistory;
	filters = new Filters;

	reset();

#ifndef NO_USE_INI_FILES
	load();
#endif
}

Preferences::~Preferences() {
#ifndef NO_USE_INI_FILES
	save();
#endif

	delete history_recents;
	delete history_urls;
	delete filters;
}

void Preferences::reset() {
    /* *******
       General
       ******* */

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
	mplayer_bin= "mplayer/mplayer.exe";
#else
	mplayer_bin = "mplayer";
#endif

	vo = ""; 
	ao = "";

	use_screenshot = true;
	screenshot_directory="";
#ifdef PORTABLE_APP
	screenshot_directory= "./screenshots";
#else
	#if QT_VERSION < 0x040400
	QString default_screenshot_path = Paths::configPath() + "/screenshots";
	if (QFile::exists(default_screenshot_path)) {
		screenshot_directory = default_screenshot_path;
	}
	#endif
#endif

	dont_remember_media_settings = false;
	dont_remember_time_pos = false;

	audio_lang = "";
	subtitle_lang = "";

	use_direct_rendering = false;
	use_double_buffer = true;

	use_soft_video_eq = false;
	use_slices = true;
	autoq = 6;
	add_blackborders_on_fullscreen = false;

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
	turn_screensaver_off = false;
	avoid_screensaver = true;
#else
	disable_screensaver = true;
#endif

#ifndef Q_OS_WIN
	vdpau.ffh264vdpau = true;
	vdpau.ffmpeg12vdpau = true;
	vdpau.ffwmv3vdpau = true;
	vdpau.ffvc1vdpau = true;
	vdpau.ffodivxvdpau = false;
	vdpau.disable_video_filters = true;
#endif

	use_soft_vol = true;
	softvol_max = 110; // 110 = default value in mplayer
	use_scaletempo = Detect;
	use_hwac3 = false;
	use_audio_equalizer = true;

	global_volume = true;
	volume = 50;
	mute = false;

	autosync = false;
	autosync_factor = 100;

	use_mc = false;
	mc_value = 0;

	osd = Seek;
	osd_delay = 2200;

	file_settings_method = "hash"; // Possible values: normal & hash


    /* ***************
       Drives (CD/DVD)
       *************** */

	dvd_device = "";
	cdrom_device = "";

#ifndef Q_OS_WIN
	// Try to set default values
	if (QFile::exists("/dev/dvd")) dvd_device = "/dev/dvd";
	if (QFile::exists("/dev/cdrom")) cdrom_device = "/dev/cdrom";
#endif

#ifdef Q_OS_WIN
	enable_audiocd_on_windows = false;
#endif

	vcd_initial_title = 2; // Most VCD's start at title #2

#if DVDNAV_SUPPORT
	use_dvdnav = false;
#endif


    /* ***********
       Performance
       *********** */

	priority = AboveNormal; // Option only for windows
	frame_drop = true;
	hard_frame_drop = false;
	coreavc = false;
	h264_skip_loop_filter = LoopEnabled;
	HD_height = 720;

	// MPlayer 1.0rc1 require restart, new versions don't
	fast_audio_change = Detect;
#if !SMART_DVD_CHAPTERS
	fast_chapter_change = false;
#endif

	threads = 1;

	cache_for_files = 0;
	cache_for_streams = 1000;
	cache_for_dvds = 0; // not recommended to use cache for dvds
	cache_for_vcds = 1000;
	cache_for_audiocds = 1000;
	cache_for_tv = 3000;

#ifdef YOUTUBE_SUPPORT
	yt_quality = RetrieveYoutubeUrl::MP4_720p;
#endif


    /* *********
       Subtitles
       ********* */

	font_file = "";
	font_name = "";
	use_fontconfig = false;
	subcp = "ISO-8859-1";
	use_enca = false;
	enca_lang = QString(QLocale::system().name()).section("_",0,0);
	font_autoscale = 1;
	subfuzziness = 1;
	autoload_sub = true;

	use_ass_subtitles = true;
	ass_line_spacing = 0;

	use_forced_subs_only = false;

	sub_visibility = true;

	subtitles_on_screenshots = false;

	use_new_sub_commands = Detect;
	change_sub_scale_should_restart = Detect;

	fast_load_sub = true;

	// ASS styles
	// Nothing to do, default values are given in
	// AssStyles constructor

	force_ass_styles = false;
	user_forced_ass_style.clear();

	freetype_support = true;


    /* ********
       Advanced
       ******** */

#if USE_ADAPTER
	adapter = -1;
#endif

#if USE_COLORKEY
	color_key = 0x020202;
#endif

	use_mplayer_window = false;

	monitor_aspect=""; // Autodetect

	use_idx = false;

	mplayer_additional_options="";
	#ifdef PORTABLE_APP
	mplayer_additional_options="-nofontconfig";
	#endif
    mplayer_additional_video_filters="";
    mplayer_additional_audio_filters="";

#ifdef LOG_MPLAYER
	log_mplayer = true;
	verbose_log = false;
	autosave_mplayer_log = false;
	mplayer_log_saveto = "";
#endif
#ifdef LOG_SMPLAYER
	log_smplayer = true;
	log_filter = ".*";
	save_smplayer_log = false;
#endif

#if REPAINT_BACKGROUND_OPTION
	// "Repaint video background" in the preferences dialog
	#ifndef Q_OS_WIN
	repaint_video_background = false;
	#else
	repaint_video_background = true;
	#endif
#endif

	use_edl_files = true;

	prefer_ipv4 = true;

	use_short_pathnames = false;

	change_video_equalizer_on_startup = true;

	use_pausing_keep_force = true;

	use_correct_pts = Detect;

	actions_to_run = "";

	show_tag_in_window_title = true;


    /* *********
       GUI stuff
       ********* */

	fullscreen = false;
	start_in_fullscreen = false;
	compact_mode = false;
	stay_on_top = NeverOnTop;
	size_factor = 100; // 100%

	resize_method = Always;

#if STYLE_SWITCHING
	style="";
#endif


#if DVDNAV_SUPPORT
	mouse_left_click_function = "dvdnav_mouse";
#else
	mouse_left_click_function = "";
#endif
	mouse_right_click_function = "show_context_menu";
	mouse_double_click_function = "fullscreen";
	mouse_middle_click_function = "mute";
	mouse_xbutton1_click_function = "";
	mouse_xbutton2_click_function = "";
	wheel_function = Seeking;
	wheel_function_cycle = Seeking | Volume | Zoom | ChangeSpeed;
	wheel_function_seeking_reverse = false;

	seeking1 = 10;
	seeking2 = 60;
	seeking3 = 10*60;
	seeking4 = 30;

	update_while_seeking = false;
#if ENABLE_DELAYED_DRAGGING
	time_slider_drag_delay = 100;
#endif
#if SEEKBAR_RESOLUTION
	relative_seeking = false;
#endif
	precise_seeking = true;

	language = "";
	iconset = "";

	balloon_count = 5;

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
	restore_pos_after_fullscreen = true;
#else
	restore_pos_after_fullscreen = false;
#endif

	save_window_size_on_exit = true;

	close_on_finish = false;

	default_font = "";

	pause_when_hidden = false;

	allow_video_movement = false;

	gui = "DefaultGui";

#if USE_MINIMUMSIZE
	gui_minimum_width = 0; // 0 == disabled
#endif
	default_size = QSize(580, 440);

#if ALLOW_TO_HIDE_VIDEO_WINDOW_ON_AUDIO_FILES
	hide_video_window_on_audio_files = true;
#endif

	report_mplayer_crashes = true;

#if REPORT_OLD_MPLAYER
	reported_mplayer_is_old = false;
#endif

	auto_add_to_playlist = true;
	add_to_playlist_consecutive_files = false;


    /* ********
       TV (dvb)
       ******** */

	check_channels_conf_on_startup = true;
	initial_tv_deinterlace = MediaSettings::Yadif_1;
	last_dvb_channel = "";
	last_tv_channel = "";


    /* ***********
       Directories
       *********** */

	latest_dir = QDir::homePath();
	last_dvd_directory="";
	save_dirs = true;

    /* **************
       Initial values
       ************** */

	initial_sub_scale = 5;
	initial_sub_scale_ass = 1;
	initial_volume = 40;
	initial_contrast = 0;
	initial_brightness = 0;
	initial_hue = 0;
	initial_saturation = 0;
	initial_gamma = 0;

	initial_audio_equalizer << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;

	initial_zoom_factor = 1.0;
	initial_sub_pos = 100; // 100%

	initial_postprocessing = false;
	initial_volnorm = false;

	initial_deinterlace = MediaSettings::NoDeinterlace;

	initial_audio_channels = MediaSettings::ChDefault;
	initial_stereo_mode = MediaSettings::Stereo;

	initial_audio_track = 1;
	initial_subtitle_track = 1;


    /* ************
       MPlayer info
       ************ */

	mplayer_detected_version = -1; //None version parsed yet
	mplayer_user_supplied_version = -1;
	mplayer_is_mplayer2 = false;
	mplayer2_detected_version = QString::null;


    /* *********
       Instances
       ********* */
#ifdef SINGLE_INSTANCE
	use_single_instance = true;
#endif


    /* ****************
       Floating control
       **************** */

	floating_control_margin = 0;
	floating_control_width = 70; //70 %
	floating_control_animated = true;
	floating_display_in_compact_mode = false;
#ifndef Q_OS_WIN
	bypass_window_manager = true;
#endif


    /* *******
       History
       ******* */

	history_recents->clear();
	history_urls->clear();


    /* *******
       Filters
       ******* */

	filters->init();
}

#ifndef NO_USE_INI_FILES
void Preferences::save() {
	qDebug("Preferences::save");

	QSettings * set = settings;


    /* *******
       General
       ******* */

	set->beginGroup( "general");

	set->setValue("mplayer_bin", mplayer_bin);
	set->setValue("driver/vo", vo);
	set->setValue("driver/audio_output", ao);

	set->setValue("use_screenshot", use_screenshot);
	#if QT_VERSION >= 0x040400
	set->setValue("screenshot_folder", screenshot_directory);
	#else
	set->setValue("screenshot_directory", screenshot_directory);
	#endif

	set->setValue("dont_remember_media_settings", dont_remember_media_settings);
	set->setValue("dont_remember_time_pos", dont_remember_time_pos);

	set->setValue("audio_lang", audio_lang);
	set->setValue("subtitle_lang", subtitle_lang);

	set->setValue("use_direct_rendering", use_direct_rendering);
	set->setValue("use_double_buffer", use_double_buffer);
	set->setValue("use_soft_video_eq", use_soft_video_eq);
	set->setValue("use_slices", use_slices );
	set->setValue("autoq", autoq);
	set->setValue("add_blackborders_on_fullscreen", add_blackborders_on_fullscreen);

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
	set->setValue("turn_screensaver_off", turn_screensaver_off);
	set->setValue("avoid_screensaver", avoid_screensaver);
#else
	set->setValue("disable_screensaver", disable_screensaver);
#endif

#ifndef Q_OS_WIN
	set->setValue("vdpau_ffh264vdpau", vdpau.ffh264vdpau);
	set->setValue("vdpau_ffmpeg12vdpau", vdpau.ffmpeg12vdpau);
	set->setValue("vdpau_ffwmv3vdpau", vdpau.ffwmv3vdpau);
	set->setValue("vdpau_ffvc1vdpau", vdpau.ffvc1vdpau);
	set->setValue("vdpau_ffodivxvdpau", vdpau.ffodivxvdpau);
	set->setValue("vdpau_disable_video_filters", vdpau.disable_video_filters);
#endif

	set->setValue("use_soft_vol", use_soft_vol);
	set->setValue("softvol_max", softvol_max);
	set->setValue("use_scaletempo", use_scaletempo);
	set->setValue("use_hwac3", use_hwac3 );
	set->setValue("use_audio_equalizer", use_audio_equalizer );

	set->setValue("global_volume", global_volume);
	set->setValue("volume", volume);
	set->setValue("mute", mute);

	set->setValue("autosync", autosync);
	set->setValue("autosync_factor", autosync_factor);

	set->setValue("use_mc", use_mc);
	set->setValue("mc_value", mc_value);

	set->setValue("osd", osd);
	set->setValue("osd_delay", osd_delay);

	set->setValue("file_settings_method", file_settings_method);

	set->endGroup(); // general


    /* ***************
       Drives (CD/DVD)
       *************** */

	set->beginGroup( "drives");

	set->setValue("dvd_device", dvd_device);
	set->setValue("cdrom_device", cdrom_device);

#ifdef Q_OS_WIN
	set->setValue("enable_audiocd_on_windows", enable_audiocd_on_windows);
#endif

	set->setValue("vcd_initial_title", vcd_initial_title);

#if DVDNAV_SUPPORT
	set->setValue("use_dvdnav", use_dvdnav);
#endif

	set->endGroup(); // drives


    /* ***********
       Performance
       *********** */

	set->beginGroup( "performance");

	set->setValue("priority", priority);
	set->setValue("frame_drop", frame_drop);
	set->setValue("hard_frame_drop", hard_frame_drop);
	set->setValue("coreavc", coreavc);
	set->setValue("h264_skip_loop_filter", h264_skip_loop_filter);
	set->setValue("HD_height", HD_height);

	set->setValue("fast_audio_change", fast_audio_change);
#if !SMART_DVD_CHAPTERS
	set->setValue("fast_chapter_change", fast_chapter_change);
#endif

	set->setValue("threads", threads);

	set->setValue("cache_for_files", cache_for_files);
	set->setValue("cache_for_streams", cache_for_streams);
	set->setValue("cache_for_dvds", cache_for_dvds);
	set->setValue("cache_for_vcds", cache_for_vcds);
	set->setValue("cache_for_audiocds", cache_for_audiocds);
	set->setValue("cache_for_tv", cache_for_tv);

#ifdef YOUTUBE_SUPPORT
	set->setValue("youtube_quality", yt_quality);
#endif

	set->endGroup(); // performance


    /* *********
       Subtitles
       ********* */

	set->beginGroup("subtitles");

	set->setValue("font_file", font_file);
	set->setValue("font_name", font_name);

	set->setValue("use_fontconfig", use_fontconfig);
	set->setValue("subcp", subcp);
	set->setValue("use_enca", use_enca);
	set->setValue("enca_lang", enca_lang);
	set->setValue("font_autoscale", font_autoscale);
	set->setValue("subfuzziness", subfuzziness);
	set->setValue("autoload_sub", autoload_sub);

	set->setValue("use_ass_subtitles", use_ass_subtitles);
	set->setValue("ass_line_spacing", ass_line_spacing);
	set->setValue("use_forced_subs_only", use_forced_subs_only);

	set->setValue("sub_visibility", sub_visibility);

	set->setValue("subtitles_on_screenshots", subtitles_on_screenshots);

	set->setValue("use_new_sub_commands", use_new_sub_commands);
	set->setValue("change_sub_scale_should_restart", change_sub_scale_should_restart);

	set->setValue("fast_load_sub", fast_load_sub);

	// ASS styles
	ass_styles.save(set);
	set->setValue("force_ass_styles", force_ass_styles);
	set->setValue("user_forced_ass_style", user_forced_ass_style);

	set->setValue("freetype_support", freetype_support);

	set->endGroup(); // subtitles


    /* ********
       Advanced
       ******** */

	set->beginGroup( "advanced");

#if USE_ADAPTER
	set->setValue("adapter", adapter);
#endif

#if USE_COLORKEY
	set->setValue("color_key", QString::number(color_key,16));
#endif

	set->setValue("use_mplayer_window", use_mplayer_window);

	set->setValue("monitor_aspect", monitor_aspect);

	set->setValue("use_idx", use_idx);

	set->setValue("mplayer_additional_options", mplayer_additional_options);
	set->setValue("mplayer_additional_video_filters", mplayer_additional_video_filters);
	set->setValue("mplayer_additional_audio_filters", mplayer_additional_audio_filters);

#ifdef LOG_MPLAYER
	set->setValue("log_mplayer", log_mplayer);
	set->setValue("verbose_log", verbose_log);
	set->setValue("autosave_mplayer_log", autosave_mplayer_log);
	set->setValue("mplayer_log_saveto", mplayer_log_saveto);
#endif
#ifdef LOG_SMPLAYER
	set->setValue("log_smplayer", log_smplayer);
	set->setValue("log_filter", log_filter);
	set->setValue("save_smplayer_log", save_smplayer_log);
#endif

#if REPAINT_BACKGROUND_OPTION
	set->setValue("repaint_video_background", repaint_video_background);
#endif

	set->setValue("use_edl_files", use_edl_files);

	set->setValue("prefer_ipv4", prefer_ipv4);

	set->setValue("use_short_pathnames", use_short_pathnames);

	set->setValue("change_video_equalizer_on_startup", change_video_equalizer_on_startup);

	set->setValue("use_pausing_keep_force", use_pausing_keep_force);

	set->setValue("correct_pts", use_correct_pts);

	set->setValue("actions_to_run", actions_to_run);

	set->setValue("show_tag_in_window_title", show_tag_in_window_title);

	set->endGroup(); // advanced


    /* *********
       GUI stuff
       ********* */

	set->beginGroup("gui");

	set->setValue("fullscreen", fullscreen);
	set->setValue("start_in_fullscreen", start_in_fullscreen);

	set->setValue("compact_mode", compact_mode);
	set->setValue("stay_on_top", (int) stay_on_top);
	set->setValue("size_factor", size_factor);
	set->setValue("resize_method", resize_method);

#if STYLE_SWITCHING
	set->setValue("style", style);
#endif

	set->setValue("mouse_left_click_function", mouse_left_click_function);
	set->setValue("mouse_right_click_function", mouse_right_click_function);
	set->setValue("mouse_double_click_function", mouse_double_click_function);
	set->setValue("mouse_middle_click_function", mouse_middle_click_function);
	set->setValue("mouse_xbutton1_click_function", mouse_xbutton1_click_function);
	set->setValue("mouse_xbutton2_click_function", mouse_xbutton2_click_function);
	set->setValue("mouse_wheel_function", wheel_function);
	set->setValue("wheel_function_cycle", (int) wheel_function_cycle);
	set->setValue("wheel_function_seeking_reverse", wheel_function_seeking_reverse);

	set->setValue("seeking1", seeking1);
	set->setValue("seeking2", seeking2);
	set->setValue("seeking3", seeking3);
	set->setValue("seeking4", seeking4);

	set->setValue("update_while_seeking", update_while_seeking);
#if ENABLE_DELAYED_DRAGGING
	set->setValue("time_slider_drag_delay", time_slider_drag_delay);
#endif
#if SEEKBAR_RESOLUTION
	set->setValue("relative_seeking", relative_seeking);
#endif
	set->setValue("precise_seeking", precise_seeking);

	set->setValue("language", language);
	set->setValue("iconset", iconset);

	set->setValue("balloon_count", balloon_count);

	set->setValue("restore_pos_after_fullscreen", restore_pos_after_fullscreen);
	set->setValue("save_window_size_on_exit", save_window_size_on_exit);

	set->setValue("close_on_finish", close_on_finish);

	set->setValue("default_font", default_font);

	set->setValue("pause_when_hidden", pause_when_hidden);

	set->setValue("allow_video_movement", allow_video_movement);

	set->setValue("gui", gui);

#if USE_MINIMUMSIZE
	set->setValue("gui_minimum_width", gui_minimum_width);
#endif
	set->setValue("default_size", default_size);

#if ALLOW_TO_HIDE_VIDEO_WINDOW_ON_AUDIO_FILES
	set->setValue("hide_video_window_on_audio_files", hide_video_window_on_audio_files);
#endif

	set->setValue("report_mplayer_crashes", report_mplayer_crashes);

#if REPORT_OLD_MPLAYER
	set->setValue("reported_mplayer_is_old", reported_mplayer_is_old);
#endif

    set->setValue("auto_add_to_playlist", auto_add_to_playlist);
    set->setValue("add_to_playlist_consecutive_files", add_to_playlist_consecutive_files);

	set->endGroup(); // gui


    /* ********
       TV (dvb)
       ******** */

	set->beginGroup( "tv");
	set->setValue("check_channels_conf_on_startup", check_channels_conf_on_startup);
	set->setValue("initial_tv_deinterlace", initial_tv_deinterlace);
	set->setValue("last_dvb_channel", last_dvb_channel);
	set->setValue("last_tv_channel", last_tv_channel);
	set->endGroup(); // tv

    /* ***********
       Directories
       *********** */

	set->beginGroup( "directories");
	if (save_dirs) {
		set->setValue("latest_dir", latest_dir);
		set->setValue("last_dvd_directory", last_dvd_directory);
	} else {
		set->setValue("latest_dir", "");
		set->setValue("last_dvd_directory", "");
	}
	set->setValue("save_dirs", save_dirs);
	set->endGroup(); // directories


    /* **************
       Initial values
       ************** */

	set->beginGroup( "defaults");

	set->setValue("initial_sub_scale", initial_sub_scale);
	set->setValue("initial_sub_scale_ass", initial_sub_scale_ass);
	set->setValue("initial_volume", initial_volume);
	set->setValue("initial_contrast", initial_contrast);
	set->setValue("initial_brightness", initial_brightness);
	set->setValue("initial_hue", initial_hue);
	set->setValue("initial_saturation", initial_saturation);
	set->setValue("initial_gamma", initial_gamma);

	set->setValue("initial_audio_equalizer", initial_audio_equalizer);

	set->setValue("initial_zoom_factor", initial_zoom_factor);
	set->setValue("initial_sub_pos", initial_sub_pos);

	set->setValue("initial_volnorm", initial_volnorm);
	set->setValue("initial_postprocessing", initial_postprocessing);

	set->setValue("initial_deinterlace", initial_deinterlace);

	set->setValue("initial_audio_channels", initial_audio_channels);
	set->setValue("initial_stereo_mode", initial_stereo_mode);

	set->setValue("initial_audio_track", initial_audio_track);
	set->setValue("initial_subtitle_track", initial_subtitle_track);

	set->endGroup(); // defaults


    /* ************
       MPlayer info
       ************ */

	set->beginGroup( "mplayer_info");
	set->setValue("mplayer_detected_version", mplayer_detected_version);
	set->setValue("mplayer_user_supplied_version", mplayer_user_supplied_version);
	set->setValue("is_mplayer2", mplayer_is_mplayer2);
	set->setValue("mplayer2_detected_version", mplayer2_detected_version);
	set->endGroup(); // mplayer_info


    /* *********
       Instances
       ********* */
#ifdef SINGLE_INSTANCE
	set->beginGroup("instances");
	set->setValue("single_instance_enabled", use_single_instance);
	set->endGroup(); // instances
#endif


    /* ****************
       Floating control
       **************** */

	set->beginGroup("floating_control");
	set->setValue("margin", floating_control_margin);
	set->setValue("width", floating_control_width);
	set->setValue("animated", floating_control_animated);
	set->setValue("display_in_compact_mode", floating_display_in_compact_mode);
#ifndef Q_OS_WIN
	set->setValue("bypass_window_manager", bypass_window_manager);
#endif
	set->endGroup(); // floating_control


    /* *******
       History
       ******* */

	set->beginGroup("history");
	set->setValue("recents", history_recents->toStringList());
	set->setValue("recents/max_items", history_recents->maxItems());
	set->setValue("urls", history_urls->toStringList());
	set->setValue("urls/max_items", history_urls->maxItems());
	set->endGroup(); // history


    /* *******
       Filters
       ******* */

	filters->save(set);


	set->sync();
}

void Preferences::load() {
	qDebug("Preferences::load");

	QSettings * set = settings;


    /* *******
       General
       ******* */

	set->beginGroup( "general");

	mplayer_bin = set->value("mplayer_bin", mplayer_bin).toString();
	vo = set->value("driver/vo", vo).toString();
	ao = set->value("driver/audio_output", ao).toString();

	use_screenshot = set->value("use_screenshot", use_screenshot).toBool();
	#if QT_VERSION >= 0x040400
	screenshot_directory = set->value("screenshot_folder", screenshot_directory).toString();
	setupScreenshotFolder();
	#else
	screenshot_directory = set->value("screenshot_directory", screenshot_directory).toString();
	#endif

	dont_remember_media_settings = set->value("dont_remember_media_settings", dont_remember_media_settings).toBool();
	dont_remember_time_pos = set->value("dont_remember_time_pos", dont_remember_time_pos).toBool();

	audio_lang = set->value("audio_lang", audio_lang).toString();
	subtitle_lang = set->value("subtitle_lang", subtitle_lang).toString();

	use_direct_rendering = set->value("use_direct_rendering", use_direct_rendering).toBool();
	use_double_buffer = set->value("use_double_buffer", use_double_buffer).toBool();
	
	use_soft_video_eq = set->value("use_soft_video_eq", use_soft_video_eq).toBool();
	use_slices = set->value("use_slices", use_slices ).toBool();
	autoq = set->value("autoq", autoq).toInt();
	add_blackborders_on_fullscreen = set->value("add_blackborders_on_fullscreen", add_blackborders_on_fullscreen).toBool();

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
	turn_screensaver_off = set->value("turn_screensaver_off", turn_screensaver_off).toBool();
	avoid_screensaver = set->value("avoid_screensaver", avoid_screensaver).toBool();
#else
	disable_screensaver = set->value("disable_screensaver", disable_screensaver).toBool();
#endif

#ifndef Q_OS_WIN
	vdpau.ffh264vdpau = set->value("vdpau_ffh264vdpau", vdpau.ffh264vdpau).toBool();
	vdpau.ffmpeg12vdpau = set->value("vdpau_ffmpeg12vdpau", vdpau.ffmpeg12vdpau).toBool();
	vdpau.ffwmv3vdpau = set->value("vdpau_ffwmv3vdpau", vdpau.ffwmv3vdpau).toBool();
	vdpau.ffvc1vdpau = set->value("vdpau_ffvc1vdpau", vdpau.ffvc1vdpau).toBool();
	vdpau.ffodivxvdpau = set->value("vdpau_ffodivxvdpau", vdpau.ffodivxvdpau).toBool();
	vdpau.disable_video_filters = set->value("vdpau_disable_video_filters", vdpau.disable_video_filters).toBool();
#endif

	use_soft_vol = set->value("use_soft_vol", use_soft_vol).toBool();
	softvol_max = set->value("softvol_max", softvol_max).toInt();
	use_scaletempo = (OptionState) set->value("use_scaletempo", use_scaletempo).toInt();
	use_hwac3 = set->value("use_hwac3", use_hwac3 ).toBool();
	use_audio_equalizer = set->value("use_audio_equalizer", use_audio_equalizer ).toBool();

	global_volume = set->value("global_volume", global_volume).toBool();
	volume = set->value("volume", volume).toInt();
	mute = set->value("mute", mute).toBool();

	autosync = set->value("autosync", autosync).toBool();
	autosync_factor = set->value("autosync_factor", autosync_factor).toInt();

	use_mc = set->value("use_mc", use_mc).toBool();
	mc_value = set->value("mc_value", mc_value).toDouble();

	osd = set->value("osd", osd).toInt();
	osd_delay = set->value("osd_delay", osd_delay).toInt();

	file_settings_method = set->value("file_settings_method", file_settings_method).toString();

	set->endGroup(); // general


    /* ***************
       Drives (CD/DVD)
       *************** */

	set->beginGroup( "drives");

	dvd_device = set->value("dvd_device", dvd_device).toString();
	cdrom_device = set->value("cdrom_device", cdrom_device).toString();

#ifdef Q_OS_WIN
	enable_audiocd_on_windows = set->value("enable_audiocd_on_windows", enable_audiocd_on_windows).toBool();
#endif

	vcd_initial_title = set->value("vcd_initial_title", vcd_initial_title ).toInt();

#if DVDNAV_SUPPORT
	use_dvdnav = set->value("use_dvdnav", use_dvdnav).toBool();
#endif

	set->endGroup(); // drives


    /* ***********
       Performance
       *********** */

	set->beginGroup( "performance");

	priority = set->value("priority", priority).toInt();
	frame_drop = set->value("frame_drop", frame_drop).toBool();
	hard_frame_drop = set->value("hard_frame_drop", hard_frame_drop).toBool();
	coreavc = set->value("coreavc", coreavc).toBool();
	h264_skip_loop_filter = (H264LoopFilter) set->value("h264_skip_loop_filter", h264_skip_loop_filter).toInt();
	HD_height = set->value("HD_height", HD_height).toInt();

	fast_audio_change = (OptionState) set->value("fast_audio_change", fast_audio_change).toInt();
#if !SMART_DVD_CHAPTERS
	fast_chapter_change = set->value("fast_chapter_change", fast_chapter_change).toBool();
#endif

	threads = set->value("threads", threads).toInt();

	cache_for_files = set->value("cache_for_files", cache_for_files).toInt();
	cache_for_streams = set->value("cache_for_streams", cache_for_streams).toInt();
	cache_for_dvds = set->value("cache_for_dvds", cache_for_dvds).toInt();
	cache_for_vcds = set->value("cache_for_vcds", cache_for_vcds).toInt();
	cache_for_audiocds = set->value("cache_for_audiocds", cache_for_audiocds).toInt();
	cache_for_tv = set->value("cache_for_tv", cache_for_tv).toInt();

#ifdef YOUTUBE_SUPPORT
	yt_quality = set->value("youtube_quality", yt_quality).toInt();
#endif

	set->endGroup(); // performance


    /* *********
       Subtitles
       ********* */

	set->beginGroup("subtitles");

	font_file = set->value("font_file", font_file).toString();
	font_name = set->value("font_name", font_name).toString();

	use_fontconfig = set->value("use_fontconfig", use_fontconfig).toBool();
	subcp = set->value("subcp", subcp).toString();
	use_enca = set->value("use_enca", use_enca).toBool();
	enca_lang = set->value("enca_lang", enca_lang).toString();
	font_autoscale = set->value("font_autoscale", font_autoscale).toInt();
	subfuzziness = set->value("subfuzziness", subfuzziness).toInt();
	autoload_sub = set->value("autoload_sub", autoload_sub).toBool();

	use_ass_subtitles = set->value("use_ass_subtitles", use_ass_subtitles).toBool();
	ass_line_spacing = set->value("ass_line_spacing", ass_line_spacing).toInt();

	use_forced_subs_only = set->value("use_forced_subs_only", use_forced_subs_only).toBool();

	sub_visibility = set->value("sub_visibility", sub_visibility).toBool();

	subtitles_on_screenshots = set->value("subtitles_on_screenshots", subtitles_on_screenshots).toBool();

	use_new_sub_commands = (OptionState) set->value("use_new_sub_commands", use_new_sub_commands).toInt();
	change_sub_scale_should_restart = (OptionState) set->value("change_sub_scale_should_restart", change_sub_scale_should_restart).toInt();

	fast_load_sub = set->value("fast_load_sub", fast_load_sub).toBool();

	// ASS styles
	ass_styles.load(set);
	force_ass_styles = set->value("force_ass_styles", force_ass_styles).toBool();
	user_forced_ass_style = set->value("user_forced_ass_style", user_forced_ass_style).toString();

	freetype_support = set->value("freetype_support", freetype_support).toBool();

	set->endGroup(); // subtitles


    /* ********
       Advanced
       ******** */

	set->beginGroup( "advanced");

#if USE_ADAPTER
	adapter = set->value("adapter", adapter).toInt();
#endif

#if USE_COLORKEY
	bool ok;
	QString color = set->value("color_key", QString::number(color_key,16)).toString();
	unsigned int temp_color_key = color.toUInt(&ok, 16);
	if (ok)
		color_key = temp_color_key;
	//color_key = set->value("color_key", color_key).toInt();
#endif

	use_mplayer_window = set->value("use_mplayer_window", use_mplayer_window).toBool();

	monitor_aspect = set->value("monitor_aspect", monitor_aspect).toString();

	use_idx = set->value("use_idx", use_idx).toBool();

	mplayer_additional_options = set->value("mplayer_additional_options", mplayer_additional_options).toString();
	mplayer_additional_video_filters = set->value("mplayer_additional_video_filters", mplayer_additional_video_filters).toString();
	mplayer_additional_audio_filters = set->value("mplayer_additional_audio_filters", mplayer_additional_audio_filters).toString();

#ifdef LOG_MPLAYER
	log_mplayer = set->value("log_mplayer", log_mplayer).toBool();
	verbose_log = set->value("verbose_log", verbose_log).toBool();
	autosave_mplayer_log = set->value("autosave_mplayer_log", autosave_mplayer_log).toBool();
	mplayer_log_saveto = set->value("mplayer_log_saveto", mplayer_log_saveto).toString();
#endif
#ifdef LOG_SMPLAYER
	log_smplayer = set->value("log_smplayer", log_smplayer).toBool();
	log_filter = set->value("log_filter", log_filter).toString();
	save_smplayer_log = set->value("save_smplayer_log", save_smplayer_log).toBool();
#endif

#if REPAINT_BACKGROUND_OPTION
	repaint_video_background = set->value("repaint_video_background", repaint_video_background).toBool();
#endif

	use_edl_files = set->value("use_edl_files", use_edl_files).toBool();

	prefer_ipv4 = set->value("prefer_ipv4", prefer_ipv4).toBool();

	use_short_pathnames = set->value("use_short_pathnames", use_short_pathnames).toBool();

	use_pausing_keep_force = set->value("use_pausing_keep_force", use_pausing_keep_force).toBool();

	use_correct_pts = (OptionState) set->value("correct_pts", use_correct_pts).toInt();

	actions_to_run = set->value("actions_to_run", actions_to_run).toString();

	show_tag_in_window_title = set->value("show_tag_in_window_title", show_tag_in_window_title).toBool();

	set->endGroup(); // advanced


    /* *********
       GUI stuff
       ********* */

	set->beginGroup("gui");

	fullscreen = set->value("fullscreen", fullscreen).toBool();
	start_in_fullscreen = set->value("start_in_fullscreen", start_in_fullscreen).toBool();

	compact_mode = set->value("compact_mode", compact_mode).toBool();
	stay_on_top = (Preferences::OnTop) set->value("stay_on_top", (int) stay_on_top).toInt();
	size_factor = set->value("size_factor", size_factor).toInt();
	resize_method = set->value("resize_method", resize_method).toInt();

#if STYLE_SWITCHING
	style = set->value("style", style).toString();
#endif

	mouse_left_click_function = set->value("mouse_left_click_function", mouse_left_click_function).toString();
	mouse_right_click_function = set->value("mouse_right_click_function", mouse_right_click_function).toString();
	mouse_double_click_function = set->value("mouse_double_click_function", mouse_double_click_function).toString();
	mouse_middle_click_function = set->value("mouse_middle_click_function", mouse_middle_click_function).toString();
	mouse_xbutton1_click_function = set->value("mouse_xbutton1_click_function", mouse_xbutton1_click_function).toString();
	mouse_xbutton2_click_function = set->value("mouse_xbutton2_click_function", mouse_xbutton2_click_function).toString();
	wheel_function = set->value("mouse_wheel_function", wheel_function).toInt();
	int wheel_function_cycle_int = set->value("wheel_function_cycle", (int) wheel_function_cycle).toInt();
	wheel_function_cycle = QFlags<Preferences::WheelFunctions> (QFlag(wheel_function_cycle_int));
	wheel_function_seeking_reverse = set->value("wheel_function_seeking_reverse", wheel_function_seeking_reverse).toBool();

	seeking1 = set->value("seeking1", seeking1).toInt();
	seeking2 = set->value("seeking2", seeking2).toInt();
	seeking3 = set->value("seeking3", seeking3).toInt();
	seeking4 = set->value("seeking4", seeking4).toInt();

	update_while_seeking = set->value("update_while_seeking", update_while_seeking).toBool();
#if ENABLE_DELAYED_DRAGGING
	time_slider_drag_delay = set->value("time_slider_drag_delay", time_slider_drag_delay).toInt();
#endif
#if SEEKBAR_RESOLUTION
	relative_seeking = set->value("relative_seeking", relative_seeking).toBool();
#endif
	precise_seeking = set->value("precise_seeking", precise_seeking).toBool();

	language = set->value("language", language).toString();
	iconset= set->value("iconset", iconset).toString();

	balloon_count = set->value("balloon_count", balloon_count).toInt();

	restore_pos_after_fullscreen = set->value("restore_pos_after_fullscreen", restore_pos_after_fullscreen).toBool();
	save_window_size_on_exit = 	set->value("save_window_size_on_exit", save_window_size_on_exit).toBool();

	close_on_finish = set->value("close_on_finish", close_on_finish).toBool();

	default_font = set->value("default_font", default_font).toString();

	pause_when_hidden = set->value("pause_when_hidden", pause_when_hidden).toBool();

	allow_video_movement = set->value("allow_video_movement", allow_video_movement).toBool();

	gui = set->value("gui", gui).toString();

#if USE_MINIMUMSIZE
	gui_minimum_width = set->value("gui_minimum_width", gui_minimum_width).toInt();
#endif
	default_size = set->value("default_size", default_size).toSize();

#if ALLOW_TO_HIDE_VIDEO_WINDOW_ON_AUDIO_FILES
	hide_video_window_on_audio_files = set->value("hide_video_window_on_audio_files", hide_video_window_on_audio_files).toBool();
#endif

	report_mplayer_crashes = set->value("report_mplayer_crashes", report_mplayer_crashes).toBool();

#if REPORT_OLD_MPLAYER
	reported_mplayer_is_old = set->value("reported_mplayer_is_old", reported_mplayer_is_old).toBool();
#endif

	auto_add_to_playlist = set->value("auto_add_to_playlist", auto_add_to_playlist).toBool();
	add_to_playlist_consecutive_files = set->value("add_to_playlist_consecutive_files", add_to_playlist_consecutive_files).toBool();

	set->endGroup(); // gui


    /* ********
       TV (dvb)
       ******** */

	set->beginGroup( "tv");
	check_channels_conf_on_startup = set->value("check_channels_conf_on_startup", check_channels_conf_on_startup).toBool();
	initial_tv_deinterlace = set->value("initial_tv_deinterlace", initial_tv_deinterlace).toInt();
	last_dvb_channel = set->value("last_dvb_channel", last_dvb_channel).toString();
	last_tv_channel = set->value("last_tv_channel", last_tv_channel).toString();
	set->endGroup(); // tv


    /* ***********
       Directories
       *********** */

	set->beginGroup( "directories");
	save_dirs = set->value("save_dirs", save_dirs).toBool();
	if (save_dirs) {
		latest_dir = set->value("latest_dir", latest_dir).toString();
		last_dvd_directory = set->value("last_dvd_directory", last_dvd_directory).toString();
	}
	set->endGroup(); // directories


    /* **************
       Initial values
       ************** */

	set->beginGroup( "defaults");

	initial_sub_scale = set->value("initial_sub_scale", initial_sub_scale).toDouble();
	initial_sub_scale_ass = set->value("initial_sub_scale_ass", initial_sub_scale_ass).toDouble();
	initial_volume = set->value("initial_volume", initial_volume).toInt();
	initial_contrast = set->value("initial_contrast", initial_contrast).toInt();
	initial_brightness = set->value("initial_brightness", initial_brightness).toInt();
	initial_hue = set->value("initial_hue", initial_hue).toInt();
	initial_saturation = set->value("initial_saturation", initial_saturation).toInt();
	initial_gamma = set->value("initial_gamma", initial_gamma).toInt();

	initial_audio_equalizer = set->value("initial_audio_equalizer", initial_audio_equalizer).toList();

	initial_zoom_factor = set->value("initial_zoom_factor", initial_zoom_factor).toDouble();
	initial_sub_pos = set->value("initial_sub_pos", initial_sub_pos).toInt();

	initial_volnorm = set->value("initial_volnorm", initial_volnorm).toBool();
	initial_postprocessing = set->value("initial_postprocessing", initial_postprocessing).toBool();

	initial_deinterlace = set->value("initial_deinterlace", initial_deinterlace).toInt();

	initial_audio_channels = set->value("initial_audio_channels", initial_audio_channels).toInt();
	initial_stereo_mode = set->value("initial_stereo_mode", initial_stereo_mode).toInt();

	initial_audio_track = set->value("initial_audio_track", initial_audio_track).toInt();
	initial_subtitle_track = set->value("initial_subtitle_track", initial_subtitle_track).toInt();

	set->endGroup(); // defaults


    /* ************
       MPlayer info
       ************ */

	set->beginGroup( "mplayer_info");
	mplayer_detected_version = set->value("mplayer_detected_version", mplayer_detected_version).toInt();
	mplayer_user_supplied_version = set->value("mplayer_user_supplied_version", mplayer_user_supplied_version).toInt();
	mplayer_is_mplayer2 = set->value("is_mplayer2", mplayer_is_mplayer2).toBool();
	mplayer2_detected_version = set->value("mplayer2_detected_version", mplayer2_detected_version).toString();

	set->endGroup(); // mplayer_info


    /* *********
       Instances
       ********* */
#ifdef SINGLE_INSTANCE
	set->beginGroup("instances");
	use_single_instance = set->value("single_instance_enabled", use_single_instance).toBool();
	set->endGroup(); // instances
#endif


    /* ****************
       Floating control
       **************** */

	set->beginGroup("floating_control");
	floating_control_margin = set->value("margin", floating_control_margin).toInt();
	floating_control_width = set->value("width", floating_control_width).toInt();
	floating_control_animated = set->value("animated", floating_control_animated).toBool();
	floating_display_in_compact_mode = set->value("display_in_compact_mode", floating_display_in_compact_mode).toBool();
#ifndef Q_OS_WIN
	bypass_window_manager = set->value("bypass_window_manager", bypass_window_manager).toBool();
#endif
	set->endGroup(); // floating_control


    /* *******
       History
       ******* */

	set->beginGroup("history");

	history_recents->setMaxItems( set->value("recents/max_items", history_recents->maxItems()).toInt() );
	history_recents->fromStringList( set->value("recents", history_recents->toStringList()).toStringList() );

	history_urls->setMaxItems( set->value("urls/max_items", history_urls->maxItems()).toInt() );
	history_urls->fromStringList( set->value("urls", history_urls->toStringList()).toStringList() );

	set->endGroup(); // history


    /* *******
       Filters
       ******* */

	filters->load(set);
}

#endif // NO_USE_INI_FILES

double Preferences::monitor_aspect_double() {
	qDebug("Preferences::monitor_aspect_double");

	QRegExp exp("(\\d+)[:/](\\d+)");
	if (exp.indexIn( monitor_aspect ) != -1) {
		int w = exp.cap(1).toInt();
		int h = exp.cap(2).toInt();
		qDebug(" monitor_aspect parsed successfully: %d:%d", w, h);
		return (double) w/h;
	}

	bool ok;
	double res = monitor_aspect.toDouble(&ok);
	if (ok) {
		qDebug(" monitor_aspect parsed successfully: %f", res);
		return res;
	} else {
		qDebug(" warning: monitor_aspect couldn't be parsed!");
        qDebug(" monitor_aspect set to 0");
		return 0;
	}
}

void Preferences::setupScreenshotFolder() {
#if QT_VERSION >= 0x040400
	if (screenshot_directory.isEmpty()) {
		QString pdir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
		if (pdir.isEmpty()) pdir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
		if (pdir.isEmpty()) pdir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
		if (pdir.isEmpty()) pdir = "/tmp";
		if (!QFile::exists(pdir)) {
			qWarning("Preferences::setupScreenshotFolder: folder '%s' does not exist. Using /tmp as fallback", pdir.toUtf8().constData());
			pdir = "/tmp";
		}
		QString default_screenshot_path = pdir + "/smplayer_screenshots";
		if (!QFile::exists(default_screenshot_path)) {
			qDebug("Preferences::setupScreenshotFolder: creating '%s'", default_screenshot_path.toUtf8().constData());
			if (!QDir().mkdir(default_screenshot_path)) {
				qWarning("Preferences::setupScreenshotFolder: failed to create '%s'", default_screenshot_path.toUtf8().constData());
			}
		}
		if (QFile::exists(default_screenshot_path)) {
			screenshot_directory = default_screenshot_path;
		}
	}
#endif
}
