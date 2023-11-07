package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"text/template"
	"time"
)

const SERVER_PORT = 8000
const DAD_IP = "http://172.16.4.100:80"

var ROOM_STATUSES []roomStatus

type dadResponse struct {
	Name   string
	Status string
}

type roomStatus struct {
	Name   string
	Status bool
}

func getBreakoutRoomsStatus(url string) ([]roomStatus, error) {
	resp, err := http.Get(url)
	if err != nil {
		log.Fatal(fmt.Sprintf("getBreakoutRoomStatus: \nURL: %s \nError: %s", url, err.Error()))
		return nil, err
	}
	defer resp.Body.Close()
	var dadResponse []dadResponse
	err = json.NewDecoder(resp.Body).Decode(&dadResponse)
	if err != nil {
		log.Fatal(fmt.Sprintf("getBreakoutRoomStatus: \n Failed to parse JSON %s", err))
		return nil, err
	}

	var values []roomStatus
	for _, room := range dadResponse {
		if room.Status == "open" {
			values = append(values, roomStatus{room.Name, true})
		} else {
			values = append(values, roomStatus{room.Name, false})
		}
		fmt.Println(room.Status)
	}
	return values, nil
}

func setRoomStatuses() {
	for {
		ROOM_STATUSES, _ = getBreakoutRoomsStatus(DAD_IP)
		time.Sleep(time.Second * 30)
	}
}

func main() {
	go setRoomStatuses()
	mainHandler := func(w http.ResponseWriter, _ *http.Request) {
		mainTemplate := template.Must(
			template.New("index.html").ParseFiles("index.html"),
		)
		mainTemplate.Execute(w, ROOM_STATUSES)
	}
	http.HandleFunc("/", mainHandler)
	log.Fatal(http.ListenAndServe(":"+fmt.Sprint(SERVER_PORT), nil))
}
