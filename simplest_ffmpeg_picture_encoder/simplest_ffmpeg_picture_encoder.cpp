/**
 * ��򵥵Ļ���FFmpeg��ͼ�������
 * Simplest FFmpeg Picture Encoder
 * 
 * ������ Lei Xiaohua
 * leixiaohua1020@126.com
 * �й���ý��ѧ/���ֵ��Ӽ���
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 * 
 * ������ʵ����YUV420P�������ݱ���ΪJPEGͼƬ������򵥵�FFmpeg���뷽��Ľ̡̳�
 * ͨ��ѧϰ�����ӿ����˽�FFmpeg�ı������̡�
 */

#include <stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>
#ifdef __cplusplus
};
#endif
#endif


int main(int argc, char* argv[])
{
	AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* video_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;

	uint8_t* picture_buf;
	AVFrame* picture;
	AVPacket pkt;
	int y_size;
	int got_picture=0;
	int size;

	int ret=0;

	FILE *in_file = NULL;	//��ƵYUVԴ�ļ� 
	int in_w=480,in_h=272;									//���
	const char* out_file = "cuc_view_encode.jpg";					//����ļ�·��

	in_file = fopen("cuc_view_480x272.yuv", "rb");

	av_register_all();

	//����1.���ʹ�ü�������
	pFormatCtx = avformat_alloc_context();
	//�¸�ʽ����MJPEG����
	fmt = av_guess_format("mjpeg", NULL, NULL);
	pFormatCtx->oformat = fmt;
	//ע�⣺���·��
	if (avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE) < 0){
		printf("����ļ���ʧ��");
		return -1;
	}

	//����2.�����Զ���һЩ
	//avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
	//fmt = pFormatCtx->oformat;

	video_st = av_new_stream(pFormatCtx, 0);
	if (video_st==NULL){
		return -1;
	}
	pCodecCtx = video_st->codec;
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = PIX_FMT_YUVJ420P;

	pCodecCtx->width = in_w;  
	pCodecCtx->height = in_h;

	pCodecCtx->time_base.num = 1;  
	pCodecCtx->time_base.den = 25;   
	//�����ʽ��Ϣ
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec){
		printf("û���ҵ����ʵı�������");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0){
		printf("��������ʧ�ܣ�");
		return -1;
	}
	picture = avcodec_alloc_frame();
	size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	picture_buf = (uint8_t *)av_malloc(size);
	if (!picture_buf)
	{
		return -1;
	}
	avpicture_fill((AVPicture *)picture, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//д�ļ�ͷ
	avformat_write_header(pFormatCtx,NULL);

	y_size = pCodecCtx->width * pCodecCtx->height;
	av_new_packet(&pkt,y_size*3);
	//����YUV
	if (fread(picture_buf, 1, y_size*3/2, in_file) < 0)
	{
		printf("�ļ���ȡ����");
		return -1;
	}
	picture->data[0] = picture_buf;  // ����Y
	picture->data[1] = picture_buf+ y_size;  // U 
	picture->data[2] = picture_buf+ y_size*5/4; // V

	//����
	ret = avcodec_encode_video2(pCodecCtx, &pkt,picture, &got_picture);
	if(ret < 0){
		printf("�������\n");
		return -1;
	}
	if (got_picture==1){
		pkt.stream_index = video_st->index;
		ret = av_write_frame(pFormatCtx, &pkt);
	}

	av_free_packet(&pkt);
	//д�ļ�β
	av_write_trailer(pFormatCtx);

	printf("����ɹ���\n");

	if (video_st){
		avcodec_close(video_st->codec);
		av_free(picture);
		av_free(picture_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	fclose(in_file);

	return 0;
}

