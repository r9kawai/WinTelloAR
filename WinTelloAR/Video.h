/**
* @brief
* WinTelloAR Video header
* Video.h
*/
#pragma once

namespace wintelloar
{
	class Video
	{
	public:
		Video(const char* input_fpath);
		~Video();
		bool GetDecodeStatus(int &fcounter);
		cv::Mat* DoDecode();

	private:
		void OnFrameDecoded(AVFrame* frame);
		void ColorConvert(AVFrame* frame, AVFrame* rgb_frame, unsigned char* out_rgb);

		static const int VIDEO_WIDTH;
		static const int VIDEO_HEIGHT;

		bool m_decoder_start;
		int m_fcounter;
		AVFormatContext* m_format_context;
		AVStream* m_video_stream;
		AVCodec* m_codec;
		AVCodecContext* m_codec_context;
		AVFrame* m_frame;
		AVFrame* m_rgb_frame;
		AVPacket m_packet;
		cv::Mat m_decode_img;

	};
}
// EOF
