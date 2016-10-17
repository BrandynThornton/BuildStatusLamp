package main

import (
        "log"
        "time"
        "github.com/tarm/serial"
        "net/http"
        "encoding/json"
        "io/ioutil"
        "bytes"
        "os"
)

//TODO: Decouple this structure from CruiseControl.NET
type Message struct {
        Info struct {
                Name string `json:"project_name"`
                Level string `json:"level"`
                Date string `json:"latest_build_date"`
                Previous string `json:"previous_result"`
                Status string `json:"current_status"`
        } `json:"building_info"`
}

func main() {
        f, err := os.OpenFile("buildstatuslog.txt", os.O_RDWR | os.O_CREATE | os.O_APPEND, 0666)
        if err != nil {
            log.Fatalf("Error opening file: %v", err)
        }
        defer f.Close()
        log.SetOutput(f)

        var m Message
        //TODO: Get URL from args
        resp, err := http.Get("http://projectJsonUrl")
        if err != nil {
                SendSerial("#000000")
                log.Fatal(err)
        }

        defer resp.Body.Close()
        body, err := ioutil.ReadAll(resp.Body)
        if err != nil {
                SendSerial("#000000")
                log.Fatal(err)
        }

        err = json.Unmarshal(bytes.Trim(body, "[] "), &m)
        if err != nil {
                SendSerial("#000000")
                log.Fatal(err)
        }

        switch m.Info.Status {
        case "Waiting":
            switch m.Info.Previous {
            case "Passed":
                SendSerial("#00FF00")
            case "Failed":
                SendSerial("#FF0000")
            default:
                SendSerial("#000000")
                log.Fatal(m.Info.Previous)
            }
        case "Building":
            SendSerial("#FF6600")
        default:
            SendSerial("#000000")
            log.Fatal(m.Info.Status)
        }
}

func SendSerial(message string) {
      c := &serial.Config{Name: "COM3", Baud: 9600}
        s, err := serial.OpenPort(c)
        if err != nil {
                log.Fatal(err)
        }
        time.Sleep(3 * time.Second)
        log.Println("SendSerial Sending : " + message);
        n, err := s.Write([]byte(message))
        if err != nil {
                log.Fatal(err)
        }
        log.Printf("%d",n)
        err = s.Close()
        if err != nil {
                log.Fatal(err)
        }  
}