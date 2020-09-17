use ffmpeg_next::{codec, encoder, format, media, Rational};
use regex::Regex;

// https://github.com/zmwangx/rust-ffmpeg/blob/master/examples/remux.rs
pub fn remux(input_file: &str, output_file: &str) {
    let mut ictx = format::input(&input_file).unwrap();
    let mut octx = format::output(&output_file).unwrap();

    let mut stream_mapping = vec![0; ictx.nb_streams() as _];
    let mut ist_time_bases = vec![Rational(0, 1); ictx.nb_streams() as _];
    let mut ost_index = 0;
    for (ist_index, ist) in ictx.streams().enumerate() {
        let ist_medium = ist.codec().medium();
        if ist_medium != media::Type::Audio
            && ist_medium != media::Type::Video
            && ist_medium != media::Type::Subtitle
        {
            stream_mapping[ist_index] = -1;
            continue;
        }
        stream_mapping[ist_index] = ost_index;
        ist_time_bases[ist_index] = ist.time_base();
        ost_index += 1;
        let mut ost = octx.add_stream(encoder::find(codec::Id::None)).unwrap();
        ost.set_parameters(ist.parameters());
        unsafe {
            (*ost.parameters().as_mut_ptr()).codec_tag = 0;
        }
    }

    octx.set_metadata(ictx.metadata().to_owned());
    octx.write_header().unwrap();

    for (stream, mut packet) in ictx.packets() {
        let ist_index = stream.index();
        let ost_index = stream_mapping[ist_index];
        if ost_index < 0 {
            continue;
        }
        let ost = octx.stream(ost_index as _).unwrap();
        packet.rescale_ts(ist_time_bases[ist_index], ost.time_base());
        packet.set_position(-1);
        packet.set_stream(ost_index as _);
        packet.write_interleaved(&mut octx).unwrap();
    }

    octx.write_trailer().unwrap();
}

pub fn download(url: &str) {
    let re = Regex::new(r"/hls/([a-z0-9-]+)/([a-z0-9]+)/[a-z0-9-]+\.[a-z0-9]+/[a-z0-9-]+\.m3u8")
        .unwrap();
    let captures = re.captures(url).unwrap();

    let filename = &captures[1];
    let extension = &captures[2];

    let input_file = url;
    let output_file = &*format!("{}.{}", filename, extension);

    dbg!(input_file, output_file);

    remux(input_file, output_file);
}
