/** @file simpleindex.cc
 * @brief Index each paragraph of a text file as a Xapian document.
 */
/* Copyright (C) 2007,2010 Olly Betts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <xapian.h>

#include <iostream>
#include <string>

#include <stdlib.h> // For exit().

using namespace std;

int
main(int argc, char **argv)
try {
    if (argc != 2) {
	cout << "Usage: " << argv[0] << " PATH_TO_DATABASE" << endl;
	exit(1);
    }

    // Open the database for update, creating a new database if necessary.
    Xapian::WritableDatabase db(argv[1], Xapian::DB_CREATE_OR_OPEN);

    Xapian::TermGenerator indexer;
    Xapian::Stem stemmer("english");
    indexer.set_stemmer(stemmer);

    string para;
    while (true) {
	string line;
	if (cin.eof()) {
	    if (para.empty()) break;
	} else {
	    getline(cin, line);
	}

	if (line.empty()) {
	    if (!para.empty()) {
		// We've reached the end of a paragraph, so index it.
		Xapian::Document doc;
		doc.set_data(para);

		indexer.set_document(doc);
		indexer.index_text(para);

		// Add the document to the database.
		db.add_document(doc);

		para = "";
	    }
	} else {
	    if (!para.empty()) para += ' ';
	    para += line;
	}
    }

    // Explicitly flush so that we get to see any errors.  WritableDatabase's
    // destructor will flush implicitly (unless we're in a transaction) but
    // will swallow any exceptions produced.
    db.flush();
} catch (const Xapian::Error &e) {
    cout << e.get_description() << endl;
    exit(1);
}
