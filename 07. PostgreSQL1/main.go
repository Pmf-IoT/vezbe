package main

import (
	"database/sql"
	"fmt"
	_ "github.com/lib/pq"
	"log"
	"time"
)

func main() {
	// uspostavljanje konekcije
	db, err := sql.Open("postgres", "postgres://postgres:vojMisic117java@192.168.124.153:5432/iot_pmf?sslmode=disable")
	if err != nil {
		log.Fatal("Greska u konekciji sa SQL serverom.: ", err)
	}

	defer db.Close()

	// Provera komunikacije za SQL serverom
	err = db.Ping()
	if err != nil {
		log.Fatal("Greska Ping db: ", err)
	}

	fmt.Println("Uspesna konekcija na PostgreSQL server")

	// Primer select naredbe
	rows, err := db.Query("SELECT * FROM log_msg")
	if err != nil {
		log.Fatal("greska pri izvrsenju: SELECT * FROM log_msg : ", err)
	}
	defer rows.Close()

	// obrada rezultata Select upita
	for rows.Next() {
		var time_stamp time.Time
		var mac string
		var msg string

		err := rows.Scan(&time_stamp, &mac, &msg)
		if err != nil {
			log.Fatal("greska pri Scan: SELECT * FROM log_msg : ", err)
		}

		fmt.Println("Vreme: ", time_stamp, " MAC: ", mac, "Poruka: ", msg)
	}

}
