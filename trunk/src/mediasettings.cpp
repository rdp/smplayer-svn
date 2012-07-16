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

#include "mediasettings.h"
#include "preferences.h"
#include "global.h"
#include <QSettings>

using namespace Global;

MediaSettings::MediaSettings() {
	reset();
}

MediaSettings::~MediaSettings() {
}

void MediaSettings::reset() {
	current_sec = 0;
	//current_sub_id = SubNone; 
	current_sub_id = NoneSelected;
#if PROGRAM_SWITCH
	current_program_id = NoneSelected;
#endif
	current_video_id = NoneSelected;
	current_audio_id = NoneSelected;
	current_title_id = NoneSelected;
	current_chapter_id = NoneSelected;
	current_angle_id = NoneSelected;

	aspect_ratio_id = AspectAuto;

	//fullscreen = FALSE;
	volume = pref->initial_volume;
	mute = false;
	external_subtitles = "";
	external_audio = "";
	sub_delay=0;
	audio_delay=0;
	sub_pos = pref->initial_sub_pos; // 100% by default
	sub_scale = pref->initial_sub_scale; 
	sub_scale_ass = pref->initial_sub_scale_ass;

	closed_caption_channel = 0; // disabled

	brightness = pref->initial_brightness;
	contrast = pref->initial_contrast;
	gamma = pref->initial_gamma;
	hue = pref->initial_hue;
	saturation = pref->initial_saturation;

	audio_equalizer = pref->initial_audio_equalizer;

	speed = 1.0;

	phase_filter = false;
	deblock_filter = false;
	dering_filter = false;
	gradfun_filter = false;
	noise_filter = false;
	postprocessing_filter = pref->initial_postprocessing;
	upscaling_filter = false;
	current_denoiser = NoDenoise;
	current_unsharp = 0;

	//current_deinterlacer = NoDeinterlace;
	current_deinterlacer = pref->initial_deinterlace;

	add_letterbox = false;

    karaoke_filter = false;
	extrastereo_filter = false;
	volnorm_filter = pref->initial_volnorm;

	audio_use_channels = pref->initial_audio_channels; //ChDefault; // (0)
	stereo_mode = pref->initial_stereo_mode; //Stereo; // (0)

	zoom_factor = pref->initial_zoom_factor; // 1.0;

#if USE_MPLAYER_PANSCAN
	panscan_factor = 0;
#endif

	starting_time = -1; // Not set yet.

	rotate = NoRotate;
	flip = false;
	mirror = false;

	loop = false;
	A_marker = -1;
	B_marker = -1;

	is264andHD = false;

	forced_demuxer="";
    forced_video_codec="";
    forced_audio_codec="";

	original_demuxer="";
    original_video_codec="";
    original_audio_codec="";

	mplayer_additional_options="";
	mplayer_additional_video_filters="";
	mplayer_additional_audio_filters="";

	win_width=400;
	win_height=300;
}

double MediaSettings::win_aspect() {
	return (double) win_width / win_height;
}

double MediaSettings::aspectToNum(Aspect aspect) {
	double asp;

	switch (aspect) {
		case MediaSettings::AspectNone: asp = 0; break;
		case MediaSettings::Aspect43: asp = (double) 4 / 3; break;
		case MediaSettings::Aspect169: asp = (double) 16 / 9; break;
		case MediaSettings::Aspect149: asp = (double) 14 / 9; break;
		case MediaSettings::Aspect1610: asp = (double) 16 / 10; break;
		case MediaSettings::Aspect54: asp = (double) 5 / 4; break;
		case MediaSettings::Aspect235: asp = 2.35; break;
		case MediaSettings::Aspect11: asp = 1; break;
		case MediaSettings::Aspect32: asp = (double) 3 / 2; break;
		case MediaSettings::Aspect1410: asp = (double) 14 / 10; break;
		case MediaSettings::AspectAuto: asp = win_aspect(); break;
		default: asp = win_aspect(); 
                 qWarning("MediaSettings::aspectToNum: invalid aspect: %d", aspect);
	}

	return asp;
}

QString MediaSettings::aspectToString(Aspect aspect) {
	QString asp_name;

	switch (aspect) {
		case MediaSettings::AspectNone: asp_name = QObject::tr("disabled", "aspect_ratio"); break;
		case MediaSettings::Aspect43: asp_name = "4:3"; break;
		case MediaSettings::Aspect169: asp_name = "16:9"; break;
		case MediaSettings::Aspect149: asp_name = "14:9"; break;
		case MediaSettings::Aspect1610: asp_name = "16:10"; break;
		case MediaSettings::Aspect54: asp_name = "5:4"; break;
		case MediaSettings::Aspect235: asp_name = "2.35:1"; break;
		case MediaSettings::Aspect11: asp_name = "1:1"; break;
		case MediaSettings::Aspect32: asp_name = "3:2"; break;
		case MediaSettings::Aspect1410: asp_name = "14:10"; break;
		case MediaSettings::AspectAuto: asp_name = QObject::tr("auto", "aspect_ratio"); break;
		default: asp_name = QObject::tr("unknown", "aspect_ratio");
	}

	return asp_name;
}

void MediaSettings::list() {
	qDebug("MediaSettings::list");

	qDebug("  current_sec: %f", current_sec);
	qDebug("  current_sub_id: %d", current_sub_id);
#if PROGRAM_SWITCH
	qDebug("  current_program_id: %d", current_program_id);
#endif
	qDebug("  current_video_id: %d", current_video_id);
	qDebug("  current_audio_id: %d", current_audio_id);
	qDebug("  current_title_id: %d", current_title_id);
	qDebug("  current_chapter_id: %d", current_chapter_id);
	qDebug("  current_angle_id: %d", current_angle_id);

	qDebug("  aspect_ratio_id: %d", aspect_ratio_id);
	//qDebug("  fullscreen: %d", fullscreen);
	qDebug("  volume: %d", volume);
	qDebug("  mute: %d", mute);
	qDebug("  external_subtitles: '%s'", external_subtitles.toUtf8().data());
	qDebug("  external_audio: '%s'", external_audio.toUtf8().data());
	qDebug("  sub_delay: %d", sub_delay);
	qDebug("  audio_delay: %d", sub_delay);
	qDebug("  sub_pos: %d", sub_pos);
	qDebug("  sub_scale: %f", sub_scale);
	qDebug("  sub_scale_ass: %f", sub_scale_ass);

	qDebug("  closed_caption_channel: %d", closed_caption_channel);

	qDebug("  brightness: %d", brightness);
	qDebug("  contrast: %d", contrast);
	qDebug("  gamma: %d", gamma);
	qDebug("  hue: %d", hue);
	qDebug("  saturation: %d", saturation);

	qDebug("  speed: %f", speed);

	qDebug("  phase_filter: %d", phase_filter);
	qDebug("  deblock_filter: %d", deblock_filter);
	qDebug("  dering_filter: %d", dering_filter);
	qDebug("  gradfun_filter: %d", gradfun_filter);
	qDebug("  noise_filter: %d", noise_filter);
	qDebug("  postprocessing_filter: %d", postprocessing_filter);
	qDebug("  upscaling_filter: %d", upscaling_filter);
	qDebug("  current_denoiser: %d", current_denoiser);
	qDebug("  current_unsharp: %d", current_unsharp);

	qDebug("  current_deinterlacer: %d", current_deinterlacer);

	qDebug("  add_letterbox: %d", add_letterbox);

	qDebug("  karaoke_filter: %d", karaoke_filter);
	qDebug("  extrastereo_filter: %d", extrastereo_filter);
	qDebug("  volnorm_filter: %d", volnorm_filter);

	qDebug("  audio_use_channels: %d", audio_use_channels);
	qDebug("  stereo_mode: %d", stereo_mode);

	qDebug("  zoom_factor: %f", zoom_factor);

#if USE_MPLAYER_PANSCAN
	qDebug("  panscan_factor: %f", zoom_factor);
#endif

	qDebug("  rotate: %d", rotate);
	qDebug("  flip: %d", flip);
	qDebug("  mirror: %d", mirror);

	qDebug("  loop: %d", loop);
	qDebug("  A_marker: %d", A_marker);
	qDebug("  B_marker: %d", B_marker);

	qDebug("  forced_demuxer: '%s'", forced_demuxer.toUtf8().data());
	qDebug("  forced_video_codec: '%s'", forced_video_codec.toUtf8().data());
	qDebug("  forced_audio_codec: '%s'", forced_video_codec.toUtf8().data());

	qDebug("  original_demuxer: '%s'", original_demuxer.toUtf8().data());
	qDebug("  original_video_codec: '%s'", original_video_codec.toUtf8().data());
	qDebug("  original_audio_codec: '%s'", original_video_codec.toUtf8().data());

	qDebug("  mplayer_additional_options: '%s'", mplayer_additional_options.toUtf8().data());
	qDebug("  mplayer_additional_video_filters: '%s'", mplayer_additional_video_filters.toUtf8().data());
	qDebug("  mplayer_additional_audio_filters: '%s'", mplayer_additional_audio_filters.toUtf8().data());

	qDebug("  win_width: %d", win_width);
	qDebug("  win_height: %d", win_height); 
	qDebug("  win_aspect(): %f", win_aspect()); 

	qDebug("  starting_time: %f", starting_time);
	qDebug("  is264andHD: %d", is264andHD);
}

#ifndef NO_USE_INI_FILES
void MediaSettings::save(QSettings * set) {
	qDebug("MediaSettings::save");

	//QSettings * set = settings;

	/*set->beginGroup( "mediasettings" );*/

	set->setValue( "current_sec", current_sec );
	set->setValue( "current_sub_id", current_sub_id );
#if PROGRAM_SWITCH
	set->setValue( "current_program_id", current_program_id );
#endif
	set->setValue( "current_video_id", current_video_id );
	set->setValue( "current_audio_id", current_audio_id );
	set->setValue( "current_title_id", current_title_id );
	set->setValue( "current_chapter_id", current_chapter_id );
	set->setValue( "current_angle_id", current_angle_id );

	set->setValue( "aspect_ratio", aspect_ratio_id );
	//set->setValue( "fullscreen", fullscreen );
	set->setValue( "volume", volume );
	set->setValue( "mute", mute );
	set->setValue( "external_subtitles", external_subtitles );
	set->setValue( "external_audio", external_audio );
	set->setValue( "sub_delay", sub_delay);
	set->setValue( "audio_delay", audio_delay);
	set->setValue( "sub_pos", sub_pos);
	set->setValue( "sub_scale", sub_scale);
	set->setValue( "sub_scale_ass", sub_scale_ass);

	set->setValue( "closed_caption_channel", closed_caption_channel);

	set->setValue( "brightness", brightness);
	set->setValue( "contrast", contrast);
	set->setValue( "gamma", gamma);
	set->setValue( "hue", hue);
	set->setValue( "saturation", saturation);

	set->setValue("audio_equalizer", audio_equalizer );

	set->setValue( "speed", speed);

	set->setValue( "phase_filter", phase_filter);
	set->setValue( "deblock_filter", deblock_filter);
	set->setValue( "dering_filter", dering_filter);
	set->setValue( "gradfun_filter", gradfun_filter);
	set->setValue( "noise_filter", noise_filter);
	set->setValue( "postprocessing_filter", postprocessing_filter);
	set->setValue( "upscaling_filter", upscaling_filter);
	set->setValue( "current_denoiser", current_denoiser);
	set->setValue( "current_unsharp", current_unsharp);

	set->setValue( "current_deinterlacer", current_deinterlacer);

	set->setValue( "add_letterbox", add_letterbox );

	set->setValue( "karaoke_filter", karaoke_filter);
	set->setValue( "extrastereo_filter", extrastereo_filter);
	set->setValue( "volnorm_filter", volnorm_filter);

	set->setValue( "audio_use_channels", audio_use_channels);
	set->setValue( "stereo_mode", stereo_mode);

	set->setValue( "zoom_factor", zoom_factor);

#if USE_MPLAYER_PANSCAN
	set->setValue( "panscan_factor", zoom_factor);
#endif

	set->setValue( "rotate", rotate );
	set->setValue( "flip", flip);
	set->setValue( "mirror", mirror);

	set->setValue( "loop", loop);
	set->setValue( "A_marker", A_marker);
	set->setValue( "B_marker", B_marker);

	set->setValue( "forced_demuxer", forced_demuxer);
	set->setValue( "forced_video_codec", forced_video_codec);
	set->setValue( "forced_audio_codec", forced_audio_codec);

	set->setValue( "original_demuxer", original_demuxer);
	set->setValue( "original_video_codec", original_video_codec);
	set->setValue( "original_audio_codec", original_audio_codec);

	set->setValue( "mplayer_additional_options", mplayer_additional_options);
	set->setValue( "mplayer_additional_video_filters", mplayer_additional_video_filters);
	set->setValue( "mplayer_additional_audio_filters", mplayer_additional_audio_filters);

	set->setValue( "win_width", win_width );
	set->setValue( "win_height", win_height );

	set->setValue( "starting_time", starting_time );

	set->setValue( "is264andHD", is264andHD );

	/*set->endGroup();*/
}

void MediaSettings::load(QSettings * set) {
	qDebug("MediaSettings::load");

	//QSettings * set = settings;

	/*set->beginGroup( "mediasettings" );*/

	current_sec = set->value( "current_sec", current_sec).toDouble();
	current_sub_id = set->value( "current_sub_id", current_sub_id ).toInt();
#if PROGRAM_SWITCH
	current_program_id = set->value( "current_program_id", current_program_id ).toInt();
#endif
	current_video_id = set->value( "current_video_id", current_video_id ).toInt();
	current_audio_id = set->value( "current_audio_id", current_audio_id ).toInt();
	current_title_id = set->value( "current_title_id", current_title_id ).toInt();
	current_chapter_id = set->value( "current_chapter_id", current_chapter_id ).toInt();
	current_angle_id = set->value( "current_angle_id", current_angle_id ).toInt();

	aspect_ratio_id = set->value( "aspect_ratio", aspect_ratio_id ).toInt();
	//fullscreen = set->value( "fullscreen", fullscreen ).toBool();
	volume = set->value( "volume", volume ).toInt();
	mute = set->value( "mute", mute ).toBool();
	external_subtitles = set->value( "external_subtitles", external_subtitles ).toString();
	external_audio = set->value( "external_audio", external_audio ).toString();
	sub_delay = set->value( "sub_delay", sub_delay).toInt();
	audio_delay = set->value( "audio_delay", audio_delay).toInt();
	sub_pos = set->value( "sub_pos", sub_pos).toInt();
	sub_scale = set->value( "sub_scale", sub_scale).toDouble();
	sub_scale_ass = set->value( "sub_scale_ass", sub_scale_ass).toDouble();

	closed_caption_channel = set->value( "closed_caption_channel", closed_caption_channel).toInt();

	brightness = set->value( "brightness", brightness).toInt();
	contrast = set->value( "contrast", contrast).toInt();
	gamma = set->value( "gamma", gamma).toInt();
	hue = set->value( "hue", hue).toInt();
	saturation = set->value( "saturation", saturation).toInt();

	audio_equalizer = set->value("audio_equalizer", audio_equalizer ).toList();

	speed = set->value( "speed", speed ).toDouble();

	phase_filter = set->value( "phase_filter", phase_filter ).toBool();
	deblock_filter = set->value( "deblock_filter", deblock_filter).toBool();
	dering_filter = set->value( "dering_filter", dering_filter).toBool();
	gradfun_filter = set->value( "gradfun_filter", gradfun_filter).toBool();
	noise_filter = set->value( "noise_filter", noise_filter).toBool();
	postprocessing_filter = set->value( "postprocessing_filter", postprocessing_filter).toBool();
	upscaling_filter = set->value( "upscaling_filter", upscaling_filter).toBool();
	current_denoiser = set->value( "current_denoiser", current_denoiser).toInt();
	current_unsharp = set->value( "current_unsharp", current_unsharp).toInt();

	current_deinterlacer = set->value( "current_deinterlacer", current_deinterlacer ).toInt();

	add_letterbox = set->value( "add_letterbox", add_letterbox ).toBool();

	karaoke_filter = set->value( "karaoke_filter", karaoke_filter).toBool();
	extrastereo_filter = set->value( "extrastereo_filter", extrastereo_filter).toBool();
	volnorm_filter = set->value( "volnorm_filter", volnorm_filter).toBool();

	audio_use_channels = set->value( "audio_use_channels", audio_use_channels).toInt();
	stereo_mode = set->value( "stereo_mode", stereo_mode).toInt();

	zoom_factor = set->value( "zoom_factor", zoom_factor).toDouble();

#if USE_MPLAYER_PANSCAN
	panscan_factor = set->value( "panscan_factor", panscan_factor).toDouble();
#endif

	rotate = set->value( "rotate", rotate).toInt();
	flip = set->value( "flip", flip).toBool();
	mirror = set->value( "mirror", mirror).toBool();

	loop = set->value( "loop", loop).toBool();
	A_marker = set->value( "A_marker", A_marker).toInt();
	B_marker = set->value( "B_marker", B_marker).toInt();

	forced_demuxer = set->value( "forced_demuxer", forced_demuxer).toString();
	forced_video_codec = set->value( "forced_video_codec", forced_video_codec).toString();
	forced_audio_codec = set->value( "forced_audio_codec", forced_audio_codec).toString();

	original_demuxer = set->value( "original_demuxer", original_demuxer).toString();
	original_video_codec = set->value( "original_video_codec", original_video_codec).toString();
	original_audio_codec = set->value( "original_audio_codec", original_audio_codec).toString();

	mplayer_additional_options = set->value( "mplayer_additional_options", mplayer_additional_options).toString();
	mplayer_additional_video_filters = set->value( "mplayer_additional_video_filters", mplayer_additional_video_filters).toString();
	mplayer_additional_audio_filters = set->value( "mplayer_additional_audio_filters", mplayer_additional_audio_filters).toString();

	win_width = set->value( "win_width", win_width ).toInt();
	win_height = set->value( "win_height", win_height ).toInt();

	starting_time = set->value( "starting_time", starting_time ).toDouble();

	is264andHD = set->value( "is264andHD", is264andHD ).toBool();

	/*set->endGroup();*/

	// ChDefault not used anymore
	if (audio_use_channels == ChDefault) audio_use_channels = ChStereo;
}

#endif // NO_USE_INI_FILES
