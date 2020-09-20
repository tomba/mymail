#include <gmime/gmime.h>

#include "mainwindow.h"

#include <QApplication>
#include <notmuch.h>
#include <fmt/format.h>

using namespace std;

class DB;
class Query;

class Base : public enable_shared_from_this<Base>
{
public:
	virtual ~Base() = default;
};

class Message
{
public:
	Message(shared_ptr<Base> base, notmuch_message_t* msg)
		: m_base(base), m_msg(msg)
	{

	}

	string id()
	{
		return notmuch_message_get_message_id(m_msg);
	}

	string filename()
	{
		return notmuch_message_get_filename(m_msg);
	}

	shared_ptr<Base> m_base;
	notmuch_message_t* m_msg;
};

class Messages : public Base
{
public:
	Messages(shared_ptr<Base> base, notmuch_messages_t* msgs)
		: m_base(base), m_msgs(msgs)
	{

	}

	~Messages()
	{
		notmuch_messages_destroy(m_msgs);
	}

	bool valid()
	{
		return notmuch_messages_valid(m_msgs);
	}

	shared_ptr<Message> get()
	{
		notmuch_message_t* msg;
		msg = notmuch_messages_get(m_msgs);
		return make_shared<Message>(this->shared_from_this(), msg);
	}

	void next()
	{
		notmuch_messages_move_to_next(m_msgs);
	}

	shared_ptr<Base> m_base;
	notmuch_messages_t* m_msgs;
};

class Query : public Base
{
public:
	Query(shared_ptr<Base> db, notmuch_query_t* query)
		: m_db(db), m_query(query)
	{

	}

	~Query()
	{
		notmuch_query_destroy(m_query);
	}

	shared_ptr<Messages> get_messages()
	{
		notmuch_messages_t* msgs;

		notmuch_status_t stat = notmuch_query_search_messages(m_query, &msgs);

		if (stat)
			throw runtime_error("notmuch_query_search_messages failed");

		return make_shared<Messages>(this->shared_from_this(), msgs);
	}

	shared_ptr<Base> m_db;
	notmuch_query_t* m_query;
};

class DB : public Base
{
public:
	static shared_ptr<DB> open_db()
	{
		return make_shared<DB>();
	}

	DB() {
		char* cerr = nullptr;

		string db_path = "/home/tomba/mail";

		notmuch_status_t stat = notmuch_database_open_verbose(db_path.c_str(),
								      notmuch_database_mode_t::NOTMUCH_DATABASE_MODE_READ_ONLY,
								      &m_db, &cerr);
		if (stat) {
			string err(cerr);
			free(cerr);

			throw runtime_error(err);
		}
	}

	~DB()
	{
		notmuch_database_destroy(m_db);
	}

	shared_ptr<Query> query(string query)
	{
		return make_shared<Query>(this->shared_from_this(), notmuch_query_create(m_db, query.c_str()));
	}

	vector<string> get_tags()
	{
		vector<string> v;

		for (notmuch_tags_t* tags = notmuch_database_get_all_tags(m_db);
		     notmuch_tags_valid(tags);
		     notmuch_tags_move_to_next(tags))
		{
			const char* tag = notmuch_tags_get(tags);
			v.push_back(tag);
		}

		return v;
	}

	notmuch_database_t* m_db;
};

int main(int argc, char *argv[])
{
	g_mime_init();

	auto db = DB::open_db();

	for (auto t : db->get_tags())
		fmt::print("Tag {}\n", t);

	auto query = db->query("to:valkeinen subject:test");
	auto msgs = query->get_messages();
	auto msg = msgs->get();
	auto fname = msg->filename();
	fmt::print("MSG {} {}\n", msg->id(), fname);

	QApplication a(argc, argv);
	MainWindow w;
	w.setContentFile(QString::fromStdString(fname));
	w.show();
	return a.exec();
}
