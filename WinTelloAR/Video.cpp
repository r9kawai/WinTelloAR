/**
* @brief
* WinTelloAR Video source
* Video.cpp
*/
#include "std.h"
#include "Video.h"
using namespace wintelloar;

const int Video::VIDEO_WIDTH = 960;
const int Video::VIDEO_HEIGHT = 720;

/**
* @brief
*/
Video::Video(const char* input_fpath)
	:m_decoder_start(false)
	,m_fcounter(0)
	,m_format_context(NULL)
	,m_video_stream(NULL)
	,m_codec(NULL)
	,m_codec_context(NULL)
	,m_frame(av_frame_alloc())
	,m_rgb_frame(av_frame_alloc())
	,m_packet(AVPacket())
	,m_decode_img(cv::Mat::zeros(VIDEO_HEIGHT, VIDEO_WIDTH, CV_8UC3))
{
	boost::system::error_code err;
	av_register_all();

	if (avformat_open_input(&m_format_context, input_fpath, NULL, NULL) != 0) {
		std::cout << "Video::Video avformat_open_input failed" << std::endl;
		return;
	}
	if (avformat_find_stream_info(m_format_context, NULL) < 0) {
		std::cout << "Video::Video avformat_find_stream_info failed" << std::endl;
		return;
	}
	for (int i = 0; i < (int)m_format_context->nb_streams; ++i) {
		if (m_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			m_video_stream = m_format_context->streams[i];
			break;
		}
	}
	if (m_video_stream == NULL) {
		std::cout << "Video::Video No video stream" << std::endl;
		return;
	}
	m_codec = avcodec_find_decoder(m_video_stream->codecpar->codec_id);
	if (m_codec == NULL) {
		std::cout << "Video::Video No supported decoder" << std::endl;
		return;
	}
	m_codec_context = avcodec_alloc_context3(m_codec);
	if (m_codec_context == NULL) {
		std::cout << "Video::Video avcodec_alloc_context3 failed" << std::endl;
		return;
	}
	if (avcodec_parameters_to_context(m_codec_context, m_video_stream->codecpar) < 0) {
		std::cout << "Video::Video avcodec_parameters_to_context failed" << std::endl;
		return;
	}
	if (avcodec_open2(m_codec_context, m_codec, NULL) != 0) {
		std::cout << "Video::Video avcodec_open2 failed" << std::endl;
		return;
	}

	m_decoder_start = true;
	return;
}

/**
* @brief
*/
Video::~Video()
{
	if (m_decoder_start) {
		if (avcodec_send_packet(m_codec_context, nullptr) != 0) {
			std::cout << "Video::~Video::avcodec_send_packet failed" << std::endl;
		}
//		while (avcodec_receive_frame(m_codec_context, m_frame) == 0) {
//			OnFrameDecoded(m_frame);
//		}
		av_frame_free(&m_frame);
		avcodec_free_context(&m_codec_context);
		avformat_close_input(&m_format_context);
	}
	return;
}

/**
* @brief
*/
bool Video::GetDecodeStatus(int& fcounter)
{
	fcounter = m_fcounter;
	return m_decoder_start;
}

/**
* @brief
*/
cv::Mat* Video::DoDecode()
{
	if (!m_decoder_start) {
		return NULL;
	}
	bool dec = false;
	if (av_read_frame(m_format_context, &m_packet) == 0) {
		if (m_packet.stream_index == m_video_stream->index) {
			if (avcodec_send_packet(m_codec_context, &m_packet) != 0) {
				std::cout << "Video::DoDecode avcodec_send_packet failed" << std::endl;
			}
			if (avcodec_receive_frame(m_codec_context, m_frame) == 0) {
				OnFrameDecoded(m_frame);
				ColorConvert(m_frame, m_rgb_frame, m_decode_img.data);
				dec = true;
			}
		}
		av_packet_unref(&m_packet);
	}
	if (dec) {
		return &m_decode_img;
	}
	else {
		return NULL;
	}
}

/**
* @brief
*/
void Video::OnFrameDecoded(AVFrame* frame)
{
	m_fcounter++;
	std::cout << "Decoded " << m_fcounter << " " << m_frame->width << "x" << m_frame->height << " : " << m_frame->pts << std::endl;
	return;
}

/**
* @brief
*/
void Video::ColorConvert(AVFrame* frame, AVFrame* rgb_frame, unsigned char* out_rgb)
{
	int w = frame->width;
	int h = frame->height;
	int pix_fmt = frame->format;

	SwsContext* sws_context = NULL;
	sws_context = sws_getCachedContext(sws_context, w, h, (AVPixelFormat)pix_fmt, w, h, AV_PIX_FMT_BGR24, SWS_BILINEAR, NULL, NULL, NULL);
	if (!sws_context) {
		throw "H264decWrapper::cannot allocate context";
	}

	avpicture_fill((AVPicture*)rgb_frame, out_rgb, AV_PIX_FMT_RGB24, w, h);
	sws_scale(sws_context, frame->data, frame->linesize, 0, h, rgb_frame->data, rgb_frame->linesize);
	rgb_frame->width = w;
	rgb_frame->height = h;
	return;
}

// EOF