//
// Created by Giulio Carota on 17/10/21.
//
#include "demuxing/SRVideoInput.h"


/**
 * the constructor initialize the
 * input device with requested options
 */

void SRVideoInput::set(char *video_src, char *video_url, SRResolution res,int fps){
    device_url = video_url;
    device_src = video_src;
    this->fps = fps;
    char s[30];
    int value = 0;
    sprintf(s,"%d", fps);

#ifdef __APPLE__
    value = av_dict_set(&options, "pixel_format", "uyvy422", 0);
    if (value < 0) {
        cout << "\nerror in setting dictionary value";
        exit(1);
    }
    value = av_dict_set(&options, "video_device_index", "1", 0);

    if (value < 0) {
        cout << "\nerror in setting dictionary value";
        exit(1);
    }
    value = av_dict_set(&options, "capture_cursor", "1", 0);
    if (value < 0) {
        throw openSourceParameterException("Found capture_cursor value wrong while opening video input");
    }
#endif
    value = av_dict_set(&options, "framerate", s, 0);
    if (value < 0) {
        throw openSourceParameterException("Found framerate value wrong while opening video input");
    }
    if(res.width != 0 && res.height != 0 ) {
        s[0] = '\0';
        sprintf(s, "%dx%d", res.width, res.height);

        value = av_dict_set(&options, "video_size", s, 0);
        if (value < 0) {
            throw openSourceParameterException("Found video size value wrong while opening video input");
        }
    }
    value = av_dict_set(&options, "preset", "high", 0);
    if (value < 0) {
        throw openSourceParameterException("Found preset value wrong while opening video input");
    }

    value = av_dict_set(&options, "probesize", "60M", 0);
    if (value < 0) {
        throw openSourceParameterException("Found probesize value wrong while opening video input");
    }

}

AVFormatContext* SRVideoInput::open(){
    //if one of them != nullptr then input already initialized
    if(inFormatContext != nullptr || inCodecContext!= nullptr || streamIndex != -1)
        return inFormatContext;

    AVInputFormat* inVInputFormat =nullptr;
    int value = 0;

    inFormatContext = avformat_alloc_context();

    //get input format
    inVInputFormat = av_find_input_format(device_src);
    value = avformat_open_input(&inFormatContext, device_url, inVInputFormat, &options);
    if (value != 0) {
        throw openSourceException("Cannot open selected video device");
    }


    //get video stream infos from context
    value = avformat_find_stream_info(inFormatContext, nullptr);
    if (value < 0) {
        throw streamInformationException("Cannot find the stream information");
    }

    //find the first video stream with a given code
    for (int i = 0; i < inFormatContext->nb_streams; i++){
        if (inFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            streamIndex = i;
            break;
        }
    }

    if (streamIndex == -1) {
        throw streamIndexException("Cannot find the video stream index.");

    }

    AVCodecParameters *params = inFormatContext->streams[streamIndex]->codecpar;
    AVCodec *inVCodec = avcodec_find_decoder(params->codec_id);
    if (inVCodec == nullptr) {
        throw findDecoderException("Cannot find the decoder.");
    }


    inCodecContext = avcodec_alloc_context3(inVCodec);

    avcodec_parameters_to_context(inCodecContext, params);
    inCodecContext->time_base = AVRational{1, fps};
    inCodecContext->framerate = AVRational{fps,1};
    value = avcodec_open2(inCodecContext, inVCodec, nullptr);
    if (value < 0) {
        throw openAVCodecException("Cannot open the av codec.");
    }

    return inFormatContext;
}

SRResolution SRVideoInput::getInputResolution() {
    if(inCodecContext!=nullptr) {
        return {inCodecContext->width, inCodecContext->height};
    }
    else {
        throw DeviceNotOpenException("Cannot open the device.");
    };
}


