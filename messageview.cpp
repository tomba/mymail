#include <gmime/gmime.h>

#include "messageview.h"
#include <QFile>

using namespace std;

MessageView::MessageView(QWidget* parent)
	: QWebEngineView(parent)
{

}


static GMimeMessage *
parse_message (int fd)
{
	GMimeMessage *message;
	GMimeParser *parser;
	GMimeStream *stream;

	/* create a stream to read from the file descriptor */
	stream = g_mime_stream_fs_new (fd);

	/* create a new parser object to parse the stream */
	parser = g_mime_parser_new_with_stream (stream);

	/* unref the stream (parser owns a ref, so this object does not actually get free'd until we destroy the parser) */
	g_object_unref (stream);

	/* parse the message from the stream */
	message = g_mime_parser_construct_message (parser, NULL);

	/* free the parser (and the stream) */
	g_object_unref (parser);

	return message;
}

static void
write_message_to_screen (GMimeMessage *message)
{
	GMimeStream *stream;

	/* create a new stream for writing to stdout */
	stream = g_mime_stream_mem_new();
	//g_mime_stream_file_set_owner ((GMimeStreamFile *) stream, FALSE);

	/* write the message to the stream */
	g_mime_object_write_to_stream ((GMimeObject *) message, NULL, stream);

	/* flush the stream (kinda like fflush() in libc's stdio) */
	g_mime_stream_flush (stream);

	auto ba = g_mime_stream_mem_get_byte_array((GMimeStreamMem*)stream);
	printf("%s\n", ba->data);

	/* free the output stream */
	g_object_unref (stream);
}


static void
count_foreach_callback (GMimeObject *parent, GMimeObject *part, gpointer user_data)
{
	int *count = (int*)user_data;

	(*count)++;

	/* 'part' points to the current part node that
	 * g_mime_message_foreach() is iterating over */

	/* find out what class 'part' is... */
	if (GMIME_IS_MESSAGE_PART (part)) {
		/* message/rfc822 or message/news */
		GMimeMessage *message;

		/* g_mime_message_foreach() won't descend into
		   child message parts, so if we want to count any
		   subparts of this child message, we'll have to call
		   g_mime_message_foreach() again here. */

		message = g_mime_message_part_get_message ((GMimeMessagePart *) part);
		g_mime_message_foreach (message, count_foreach_callback, count);
	} else if (GMIME_IS_MESSAGE_PARTIAL (part)) {
		/* message/partial */

		/* this is an incomplete message part, probably a
		   large message that the sender has broken into
		   smaller parts and is sending us bit by bit. we
		   could save some info about it so that we could
		   piece this back together again once we get all the
		   parts? */
	} else if (GMIME_IS_MULTIPART (part)) {
		/* multipart/mixed, multipart/alternative,
		 * multipart/related, multipart/signed,
		 * multipart/encrypted, etc... */

		/* we'll get to finding out if this is a
		 * signed/encrypted multipart later... */
	} else if (GMIME_IS_PART (part)) {
		/* a normal leaf part, could be text/plain or
		 * image/jpeg etc */
	} else {
		g_assert_not_reached ();
	}
}

static void
count_parts_in_message (GMimeMessage *message)
{
	int count = 0;

	/* count the number of parts (recursively) in the message
	 * including the container multiparts */
	g_mime_message_foreach (message, count_foreach_callback, &count);

	printf ("There are %d parts in the message\n", count);
}


void MessageView::setContentFile(const QString& fname)
{
	{
	QFile f(fname);
	f.open(QIODevice::ReadOnly);

	GMimeMessage* msg = parse_message(f.handle());
	if (!msg)
		throw runtime_error("failed to parse msg");

	count_parts_in_message(msg);

	write_message_to_screen(msg);
	g_object_unref(msg);
	}

	{
	QFile f(fname);
	f.open(QIODevice::ReadOnly);
	QByteArray content = f.readAll();
	setContent(content);
	}
}
