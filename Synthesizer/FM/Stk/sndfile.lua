ffi = require('ffi')
ffi.cdef[[

enum
{	/* Major formats. */
	SF_FORMAT_WAV			= 0x010000,		/* Microsoft WAV format (little endian default). */
	SF_FORMAT_AIFF			= 0x020000,		/* Apple/SGI AIFF format (big endian). */
	SF_FORMAT_AU			= 0x030000,		/* Sun/NeXT AU format (big endian). */
	SF_FORMAT_RAW			= 0x040000,		/* RAW PCM data. */
	SF_FORMAT_PAF			= 0x050000,		/* Ensoniq PARIS file format. */
	SF_FORMAT_SVX			= 0x060000,		/* Amiga IFF / SVX8 / SV16 format. */
	SF_FORMAT_NIST			= 0x070000,		/* Sphere NIST format. */
	SF_FORMAT_VOC			= 0x080000,		/* VOC files. */
	SF_FORMAT_IRCAM			= 0x0A0000,		/* Berkeley/IRCAM/CARL */
	SF_FORMAT_W64			= 0x0B0000,		/* Sonic Foundry's 64 bit RIFF/WAV */
	SF_FORMAT_MAT4			= 0x0C0000,		/* Matlab (tm) V4.2 / GNU Octave 2.0 */
	SF_FORMAT_MAT5			= 0x0D0000,		/* Matlab (tm) V5.0 / GNU Octave 2.1 */
	SF_FORMAT_PVF			= 0x0E0000,		/* Portable Voice Format */
	SF_FORMAT_XI			= 0x0F0000,		/* Fasttracker 2 Extended Instrument */
	SF_FORMAT_HTK			= 0x100000,		/* HMM Tool Kit format */
	SF_FORMAT_SDS			= 0x110000,		/* Midi Sample Dump Standard */
	SF_FORMAT_AVR			= 0x120000,		/* Audio Visual Research */
	SF_FORMAT_WAVEX			= 0x130000,		/* MS WAVE with WAVEFORMATEX */
	SF_FORMAT_SD2			= 0x160000,		/* Sound Designer 2 */
	SF_FORMAT_FLAC			= 0x170000,		/* FLAC lossless file format */
	SF_FORMAT_CAF			= 0x180000,		/* Core Audio File format */
	SF_FORMAT_WVE			= 0x190000,		/* Psion WVE format */
	SF_FORMAT_OGG			= 0x200000,		/* Xiph OGG container */
	SF_FORMAT_MPC2K			= 0x210000,		/* Akai MPC 2000 sampler */
	SF_FORMAT_RF64			= 0x220000,		/* RF64 WAV file */
	SF_FORMAT_MPEG			= 0x230000,		/* MPEG-1/2 audio stream */

	/* Subtypes from here on. */

	SF_FORMAT_PCM_S8		= 0x0001,		/* Signed 8 bit data */
	SF_FORMAT_PCM_16		= 0x0002,		/* Signed 16 bit data */
	SF_FORMAT_PCM_24		= 0x0003,		/* Signed 24 bit data */
	SF_FORMAT_PCM_32		= 0x0004,		/* Signed 32 bit data */

	SF_FORMAT_PCM_U8		= 0x0005,		/* Unsigned 8 bit data (WAV and RAW only) */

	SF_FORMAT_FLOAT			= 0x0006,		/* 32 bit float data */
	SF_FORMAT_DOUBLE		= 0x0007,		/* 64 bit float data */

	SF_FORMAT_ULAW			= 0x0010,		/* U-Law encoded. */
	SF_FORMAT_ALAW			= 0x0011,		/* A-Law encoded. */
	SF_FORMAT_IMA_ADPCM		= 0x0012,		/* IMA ADPCM. */
	SF_FORMAT_MS_ADPCM		= 0x0013,		/* Microsoft ADPCM. */

	SF_FORMAT_GSM610		= 0x0020,		/* GSM 6.10 encoding. */
	SF_FORMAT_VOX_ADPCM		= 0x0021,		/* OKI / Dialogix ADPCM */

	SF_FORMAT_NMS_ADPCM_16	= 0x0022,		/* 16kbs NMS G721-variant encoding. */
	SF_FORMAT_NMS_ADPCM_24	= 0x0023,		/* 24kbs NMS G721-variant encoding. */
	SF_FORMAT_NMS_ADPCM_32	= 0x0024,		/* 32kbs NMS G721-variant encoding. */

	SF_FORMAT_G721_32		= 0x0030,		/* 32kbs G721 ADPCM encoding. */
	SF_FORMAT_G723_24		= 0x0031,		/* 24kbs G723 ADPCM encoding. */
	SF_FORMAT_G723_40		= 0x0032,		/* 40kbs G723 ADPCM encoding. */

	SF_FORMAT_DWVW_12		= 0x0040, 		/* 12 bit Delta Width Variable Word encoding. */
	SF_FORMAT_DWVW_16		= 0x0041, 		/* 16 bit Delta Width Variable Word encoding. */
	SF_FORMAT_DWVW_24		= 0x0042, 		/* 24 bit Delta Width Variable Word encoding. */
	SF_FORMAT_DWVW_N		= 0x0043, 		/* N bit Delta Width Variable Word encoding. */

	SF_FORMAT_DPCM_8		= 0x0050,		/* 8 bit differential PCM (XI only) */
	SF_FORMAT_DPCM_16		= 0x0051,		/* 16 bit differential PCM (XI only) */

	SF_FORMAT_VORBIS		= 0x0060,		/* Xiph Vorbis encoding. */
	SF_FORMAT_OPUS			= 0x0064,		/* Xiph/Skype Opus encoding. */

	SF_FORMAT_ALAC_16		= 0x0070,		/* Apple Lossless Audio Codec (16 bit). */
	SF_FORMAT_ALAC_20		= 0x0071,		/* Apple Lossless Audio Codec (20 bit). */
	SF_FORMAT_ALAC_24		= 0x0072,		/* Apple Lossless Audio Codec (24 bit). */
	SF_FORMAT_ALAC_32		= 0x0073,		/* Apple Lossless Audio Codec (32 bit). */

	SF_FORMAT_MPEG_LAYER_I	= 0x0080,		/* MPEG-1 Audio Layer I */
	SF_FORMAT_MPEG_LAYER_II	= 0x0081,		/* MPEG-1 Audio Layer II */
	SF_FORMAT_MPEG_LAYER_III = 0x0082,		/* MPEG-2 Audio Layer III */

	/* Endian-ness options. */

	SF_ENDIAN_FILE			= 0x00000000,	/* Default file endian-ness. */
	SF_ENDIAN_LITTLE		= 0x10000000,	/* Force little endian-ness. */
	SF_ENDIAN_BIG			= 0x20000000,	/* Force big endian-ness. */
	SF_ENDIAN_CPU			= 0x30000000,	/* Force CPU endian-ness. */

	SF_FORMAT_SUBMASK		= 0x0000FFFF,
	SF_FORMAT_TYPEMASK		= 0x0FFF0000,
	SF_FORMAT_ENDMASK		= 0x30000000
} ;

enum
{	SFC_GET_LIB_VERSION				= 0x1000,
	SFC_GET_LOG_INFO				= 0x1001,
	SFC_GET_CURRENT_SF_INFO			= 0x1002,


	SFC_GET_NORM_DOUBLE				= 0x1010,
	SFC_GET_NORM_FLOAT				= 0x1011,
	SFC_SET_NORM_DOUBLE				= 0x1012,
	SFC_SET_NORM_FLOAT				= 0x1013,
	SFC_SET_SCALE_FLOAT_INT_READ	= 0x1014,
	SFC_SET_SCALE_INT_FLOAT_WRITE	= 0x1015,

	SFC_GET_SIMPLE_FORMAT_COUNT		= 0x1020,
	SFC_GET_SIMPLE_FORMAT			= 0x1021,

	SFC_GET_FORMAT_INFO				= 0x1028,

	SFC_GET_FORMAT_MAJOR_COUNT		= 0x1030,
	SFC_GET_FORMAT_MAJOR			= 0x1031,
	SFC_GET_FORMAT_SUBTYPE_COUNT	= 0x1032,
	SFC_GET_FORMAT_SUBTYPE			= 0x1033,

	SFC_CALC_SIGNAL_MAX				= 0x1040,
	SFC_CALC_NORM_SIGNAL_MAX		= 0x1041,
	SFC_CALC_MAX_ALL_CHANNELS		= 0x1042,
	SFC_CALC_NORM_MAX_ALL_CHANNELS	= 0x1043,
	SFC_GET_SIGNAL_MAX				= 0x1044,
	SFC_GET_MAX_ALL_CHANNELS		= 0x1045,

	SFC_SET_ADD_PEAK_CHUNK			= 0x1050,

	SFC_UPDATE_HEADER_NOW			= 0x1060,
	SFC_SET_UPDATE_HEADER_AUTO		= 0x1061,

	SFC_FILE_TRUNCATE				= 0x1080,

	SFC_SET_RAW_START_OFFSET		= 0x1090,

	/* Commands reserved for dithering, which is not implemented. */
	SFC_SET_DITHER_ON_WRITE			= 0x10A0,
	SFC_SET_DITHER_ON_READ			= 0x10A1,

	SFC_GET_DITHER_INFO_COUNT		= 0x10A2,
	SFC_GET_DITHER_INFO				= 0x10A3,

	SFC_GET_EMBED_FILE_INFO			= 0x10B0,

	SFC_SET_CLIPPING				= 0x10C0,
	SFC_GET_CLIPPING				= 0x10C1,

	SFC_GET_CUE_COUNT				= 0x10CD,
	SFC_GET_CUE						= 0x10CE,
	SFC_SET_CUE						= 0x10CF,

	SFC_GET_INSTRUMENT				= 0x10D0,
	SFC_SET_INSTRUMENT				= 0x10D1,

	SFC_GET_LOOP_INFO				= 0x10E0,

	SFC_GET_BROADCAST_INFO			= 0x10F0,
	SFC_SET_BROADCAST_INFO			= 0x10F1,

	SFC_GET_CHANNEL_MAP_INFO		= 0x1100,
	SFC_SET_CHANNEL_MAP_INFO		= 0x1101,

	SFC_RAW_DATA_NEEDS_ENDSWAP		= 0x1110,

	/* Support for Wavex Ambisonics Format */
	SFC_WAVEX_SET_AMBISONIC			= 0x1200,
	SFC_WAVEX_GET_AMBISONIC			= 0x1201,

	/*
	** RF64 files can be set so that on-close, writable files that have less
	** than 4GB of data in them are converted to RIFF/WAV, as per EBU
	** recommendations.
	*/
	SFC_RF64_AUTO_DOWNGRADE			= 0x1210,

	SFC_SET_VBR_ENCODING_QUALITY	= 0x1300,
	SFC_SET_COMPRESSION_LEVEL		= 0x1301,

	/* Ogg format commands */
	SFC_SET_OGG_PAGE_LATENCY_MS		= 0x1302,
	SFC_SET_OGG_PAGE_LATENCY		= 0x1303,
	SFC_GET_OGG_STREAM_SERIALNO		= 0x1306,

	SFC_GET_BITRATE_MODE			= 0x1304,
	SFC_SET_BITRATE_MODE			= 0x1305,

	/* Cart Chunk support */
	SFC_SET_CART_INFO				= 0x1400,
	SFC_GET_CART_INFO				= 0x1401,

	/* Opus files original samplerate metadata */
	SFC_SET_ORIGINAL_SAMPLERATE		= 0x1500,
	SFC_GET_ORIGINAL_SAMPLERATE		= 0x1501,

	/* Following commands for testing only. */
	SFC_TEST_IEEE_FLOAT_REPLACE		= 0x6001,

	/*
	** These SFC_SET_ADD_* values are deprecated and will disappear at some
	** time in the future. They are guaranteed to be here up to and
	** including version 1.0.8 to avoid breakage of existing software.
	** They currently do nothing and will continue to do nothing.
	*/
	SFC_SET_ADD_HEADER_PAD_CHUNK	= 0x1051,

	SFC_SET_ADD_DITHER_ON_WRITE		= 0x1070,
	SFC_SET_ADD_DITHER_ON_READ		= 0x1071
} ;


/*
** String types that can be set and read from files. Not all file types
** support this and even the file types which support one, may not support
** all string types.
*/

enum
{	SF_STR_TITLE					= 0x01,
	SF_STR_COPYRIGHT				= 0x02,
	SF_STR_SOFTWARE					= 0x03,
	SF_STR_ARTIST					= 0x04,
	SF_STR_COMMENT					= 0x05,
	SF_STR_DATE						= 0x06,
	SF_STR_ALBUM					= 0x07,
	SF_STR_LICENSE					= 0x08,
	SF_STR_TRACKNUMBER				= 0x09,
	SF_STR_GENRE					= 0x10
} ;

enum
{	/* True and false */
	SF_FALSE	= 0,
	SF_TRUE		= 1,

	/* Modes for opening files. */
	SFM_READ	= 0x10,
	SFM_WRITE	= 0x20,
	SFM_RDWR	= 0x30,

	SF_AMBISONIC_NONE		= 0x40,
	SF_AMBISONIC_B_FORMAT	= 0x41
} ;


enum
{	SF_ERR_NO_ERROR				= 0,
	SF_ERR_UNRECOGNISED_FORMAT	= 1,
	SF_ERR_SYSTEM				= 2,
	SF_ERR_MALFORMED_FILE		= 3,
	SF_ERR_UNSUPPORTED_ENCODING	= 4
} ;

enum
{	SF_CHANNEL_MAP_INVALID = 0,
	SF_CHANNEL_MAP_MONO = 1,
	SF_CHANNEL_MAP_LEFT,					/* Apple calls this 'Left' */
	SF_CHANNEL_MAP_RIGHT,					/* Apple calls this 'Right' */
	SF_CHANNEL_MAP_CENTER,					/* Apple calls this 'Center' */
	SF_CHANNEL_MAP_FRONT_LEFT,
	SF_CHANNEL_MAP_FRONT_RIGHT,
	SF_CHANNEL_MAP_FRONT_CENTER,
	SF_CHANNEL_MAP_REAR_CENTER,				/* Apple calls this 'Center Surround', Msft calls this 'Back Center' */
	SF_CHANNEL_MAP_REAR_LEFT,				/* Apple calls this 'Left Surround', Msft calls this 'Back Left' */
	SF_CHANNEL_MAP_REAR_RIGHT,				/* Apple calls this 'Right Surround', Msft calls this 'Back Right' */
	SF_CHANNEL_MAP_LFE,						/* Apple calls this 'LFEScreen', Msft calls this 'Low Frequency'  */
	SF_CHANNEL_MAP_FRONT_LEFT_OF_CENTER,	/* Apple calls this 'Left Center' */
	SF_CHANNEL_MAP_FRONT_RIGHT_OF_CENTER,	/* Apple calls this 'Right Center */
	SF_CHANNEL_MAP_SIDE_LEFT,				/* Apple calls this 'Left Surround Direct' */
	SF_CHANNEL_MAP_SIDE_RIGHT,				/* Apple calls this 'Right Surround Direct' */
	SF_CHANNEL_MAP_TOP_CENTER,				/* Apple calls this 'Top Center Surround' */
	SF_CHANNEL_MAP_TOP_FRONT_LEFT,			/* Apple calls this 'Vertical Height Left' */
	SF_CHANNEL_MAP_TOP_FRONT_RIGHT,			/* Apple calls this 'Vertical Height Right' */
	SF_CHANNEL_MAP_TOP_FRONT_CENTER,		/* Apple calls this 'Vertical Height Center' */
	SF_CHANNEL_MAP_TOP_REAR_LEFT,			/* Apple and MS call this 'Top Back Left' */
	SF_CHANNEL_MAP_TOP_REAR_RIGHT,			/* Apple and MS call this 'Top Back Right' */
	SF_CHANNEL_MAP_TOP_REAR_CENTER,			/* Apple and MS call this 'Top Back Center' */

	SF_CHANNEL_MAP_AMBISONIC_B_W,
	SF_CHANNEL_MAP_AMBISONIC_B_X,
	SF_CHANNEL_MAP_AMBISONIC_B_Y,
	SF_CHANNEL_MAP_AMBISONIC_B_Z,

	SF_CHANNEL_MAP_MAX
} ;

enum
{	SF_BITRATE_MODE_CONSTANT = 0,
	SF_BITRATE_MODE_AVERAGE,
	SF_BITRATE_MODE_VARIABLE
} ;

typedef	struct sf_private_tag	SNDFILE ;
typedef int64_t sf_count_t;

struct SF_INFO
{	sf_count_t	frames ;		/* Used to be called samples.  Changed to avoid confusion. */
	int			samplerate ;
	int			channels ;
	int			format ;
	int			sections ;
	int			seekable ;
} ;

typedef	struct SF_INFO SF_INFO ;

/* The SF_FORMAT_INFO struct is used to retrieve information about the sound
** file formats libsndfile supports using the sf_command () interface.
**
** Using this interface will allow applications to support new file formats
** and encoding types when libsndfile is upgraded, without requiring
** re-compilation of the application.
**
** Please consult the libsndfile documentation (particularly the information
** on the sf_command () interface) for examples of its use.
*/

typedef struct
{	int			format ;
	const char	*name ;
	const char	*extension ;
} SF_FORMAT_INFO ;

/*
** Enums and typedefs for adding dither on read and write.
** Reserved for future implementation.
*/

enum
{	SFD_DEFAULT_LEVEL	= 0,
	SFD_CUSTOM_LEVEL	= 0x40000000,

	SFD_NO_DITHER		= 500,
	SFD_WHITE			= 501,
	SFD_TRIANGULAR_PDF	= 502
} ;


typedef struct
{	int			type ;
	double		level ;
	const char	*name ;
} SF_DITHER_INFO ;

typedef struct
{	sf_count_t	offset ;
	sf_count_t	length ;
} SF_EMBED_FILE_INFO ;


typedef struct
{	int32_t 	indx ;
	uint32_t 	position ;
	int32_t 	fcc_chunk ;
	int32_t 	chunk_start ;
	int32_t		block_start ;
	uint32_t 	sample_offset ;
	char name [256] ;
} SF_CUE_POINT ;

enum
{	/*
	**	The loop mode field in SF_INSTRUMENT will be one of the following.
	*/
	SF_LOOP_NONE = 800,
	SF_LOOP_FORWARD,
	SF_LOOP_BACKWARD,
	SF_LOOP_ALTERNATING
} ;

typedef struct
{	int gain ;
	char basenote, detune ;
	char velocity_lo, velocity_hi ;
	char key_lo, key_hi ;
	int loop_count ;

	struct
	{	int mode ;
		uint32_t start ;
		uint32_t end ;
		uint32_t count ;
	} loops [16] ; /* make variable in a sensible way */
} SF_INSTRUMENT ;

typedef struct
{
	short	time_sig_num ;	/* any positive integer    > 0  */
	short	time_sig_den ;	/* any positive power of 2 > 0  */
	int		loop_mode ;		/* see SF_LOOP enum             */

	int		num_beats ;		/* this is NOT the amount of quarter notes !!!*/
							/* a full bar of 4/4 is 4 beats */
							/* a full bar of 7/8 is 7 beats */

	float	bpm ;			/* suggestion, as it can be calculated using other fields:*/
							/* file's length, file's sampleRate and our time_sig_den*/
							/* -> bpms are always the amount of _quarter notes_ per minute */

	int	root_key ;			/* MIDI note, or -1 for None */
	int future [6] ;
} SF_LOOP_INFO ;

typedef sf_count_t		(*sf_vio_get_filelen)	(void *user_data) ;
typedef sf_count_t		(*sf_vio_seek)		(sf_count_t offset, int whence, void *user_data) ;
typedef sf_count_t		(*sf_vio_read)		(void *ptr, sf_count_t count, void *user_data) ;
typedef sf_count_t		(*sf_vio_write)		(const void *ptr, sf_count_t count, void *user_data) ;
typedef sf_count_t		(*sf_vio_tell)		(void *user_data) ;

struct SF_VIRTUAL_IO
{	sf_vio_get_filelen	get_filelen ;
	sf_vio_seek			seek ;
	sf_vio_read			read ;
	sf_vio_write		write ;
	sf_vio_tell			tell ;
} ;

typedef	struct SF_VIRTUAL_IO SF_VIRTUAL_IO ;

SNDFILE* 	sf_open		(const char *path, int mode, SF_INFO *sfinfo) ;
SNDFILE* 	sf_open_fd	(int fd, int mode, SF_INFO *sfinfo, int close_desc) ;
SNDFILE* 	sf_open_virtual	(SF_VIRTUAL_IO *sfvirtual, int mode, SF_INFO *sfinfo, void *user_data) ;
int		sf_error		(SNDFILE *sndfile) ;
const char* sf_strerror (SNDFILE *sndfile) ;
const char*	sf_error_number	(int errnum) ;
int		sf_perror		(SNDFILE *sndfile) ;
int		sf_error_str	(SNDFILE *sndfile, char* str, size_t len) ;
int		sf_command	(SNDFILE *sndfile, int command, void *data, int datasize) ;
int		sf_format_check	(const SF_INFO *info) ;

enum
{	SF_SEEK_SET = 0,
	SF_SEEK_CUR = 1,
	SF_SEEK_END = 2
} ;

sf_count_t	sf_seek 		(SNDFILE *sndfile, sf_count_t frames, int whence) ;

int sf_set_string (SNDFILE *sndfile, int str_type, const char* str) ;
const char* sf_get_string (SNDFILE *sndfile, int str_type) ;
const char * sf_version_string (void) ;

int sf_current_byterate (SNDFILE *sndfile) ;

sf_count_t	sf_read_raw		(SNDFILE *sndfile, void *ptr, sf_count_t bytes) ;
sf_count_t	sf_write_raw 	(SNDFILE *sndfile, const void *ptr, sf_count_t bytes) ;

sf_count_t	sf_readf_short	(SNDFILE *sndfile, short *ptr, sf_count_t frames) ;
sf_count_t	sf_writef_short	(SNDFILE *sndfile, const short *ptr, sf_count_t frames) ;

sf_count_t	sf_readf_int	(SNDFILE *sndfile, int *ptr, sf_count_t frames) ;
sf_count_t	sf_writef_int 	(SNDFILE *sndfile, const int *ptr, sf_count_t frames) ;

sf_count_t	sf_readf_float	(SNDFILE *sndfile, float *ptr, sf_count_t frames) ;
sf_count_t	sf_writef_float	(SNDFILE *sndfile, const float *ptr, sf_count_t frames) ;

sf_count_t	sf_readf_double		(SNDFILE *sndfile, double *ptr, sf_count_t frames) ;
sf_count_t	sf_writef_double	(SNDFILE *sndfile, const double *ptr, sf_count_t frames) ;

sf_count_t	sf_read_short	(SNDFILE *sndfile, short *ptr, sf_count_t items) ;
sf_count_t	sf_write_short	(SNDFILE *sndfile, const short *ptr, sf_count_t items) ;

sf_count_t	sf_read_int		(SNDFILE *sndfile, int *ptr, sf_count_t items) ;
sf_count_t	sf_write_int 	(SNDFILE *sndfile, const int *ptr, sf_count_t items) ;

sf_count_t	sf_read_float	(SNDFILE *sndfile, float *ptr, sf_count_t items) ;
sf_count_t	sf_write_float	(SNDFILE *sndfile, const float *ptr, sf_count_t items) ;

sf_count_t	sf_read_double	(SNDFILE *sndfile, double *ptr, sf_count_t items) ;
sf_count_t	sf_write_double	(SNDFILE *sndfile, const double *ptr, sf_count_t items) ;

int		sf_close		(SNDFILE *sndfile) ;
void	sf_write_sync	(SNDFILE *sndfile) ;

struct SF_CHUNK_INFO
{	char		id [64] ;	/* The chunk identifier. */
	unsigned	id_size ;	/* The size of the chunk identifier. */
	unsigned	datalen ;	/* The size of that data. */
	void		*data ;		/* Pointer to the data. */
} ;

typedef struct SF_CHUNK_INFO SF_CHUNK_INFO ;

int sf_set_chunk (SNDFILE * sndfile, const SF_CHUNK_INFO * chunk_info) ;
typedef	struct SF_CHUNK_ITERATOR SF_CHUNK_ITERATOR ;

SF_CHUNK_ITERATOR *sf_get_chunk_iterator (SNDFILE * sndfile, const SF_CHUNK_INFO * chunk_info) ;

SF_CHUNK_ITERATOR *
sf_next_chunk_iterator (SF_CHUNK_ITERATOR * iterator) ;

int sf_get_chunk_size (const SF_CHUNK_ITERATOR * it, SF_CHUNK_INFO * chunk_info) ;
int sf_get_chunk_data (const SF_CHUNK_ITERATOR * it, SF_CHUNK_INFO * chunk_info) ;

void * malloc(size_t size);

]]

sndfile = ffi.load('libsndfile.so')


info = ffi.new("struct SF_INFO")
info["format"]=0
file = sndfile.sf_open("test.wav",sndfile.SFM_READ,info)
print(info["format"])
buffer = ffi.C.malloc(1024*4)
size = sndfile.sf_read_float(file,buffer,1024)
print(size)



