#! /bin/sh
#��򵥵Ļ���FFmpeg��ͼ�������----�����б���
#Simplest FFmpeg Picture Encoder----Compile in Shell 
#
#������ Lei Xiaohua
#leixiaohua1020@126.com
#�й���ý��ѧ/���ֵ��Ӽ���
#Communication University of China / Digital TV Technology
#http://blog.csdn.net/leixiaohua1020
#
#compile
gcc simplest_ffmpeg_picture_encoder.cpp -g -o simplest_ffmpeg_picture_encoder.out \
-I /usr/local/include -L /usr/local/lib -lavformat -lavcodec -lavutil
