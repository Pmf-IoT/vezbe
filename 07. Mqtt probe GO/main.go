package main

import (
	"fmt"
	mqtt "github.com/eclipse/paho.mqtt.golang"
	"math/rand"
	"os"
	"os/signal"
	"syscall"
	"time"
)

const (
	Broker             = "tcp://broker.emqx.io:1883"
	cc_topic_from_temp = "/sensor/from/temp/#"
)

var messagePubHandler mqtt.MessageHandler = func(client mqtt.Client, msg mqtt.Message) {
	fmt.Printf("Received message: %s from topic: %s\n", msg.Payload(), msg.Topic())
}

var connectHandler mqtt.OnConnectHandler = func(client mqtt.Client) {
	fmt.Println("Connected")
}

var connectLostHandler mqtt.ConnectionLostHandler = func(client mqtt.Client, err error) {
	fmt.Printf("Connect lost: %v", err)
}

func main() {
	opts := mqtt.NewClientOptions()
	opts.AddBroker(Broker)
	opts.SetClientID(fmt.Sprintf("go_mqtt_client_%d", rand.Intn(10000)))
	opts.SetUsername("emqx")
	opts.SetPassword("emqx")
	opts.SetDefaultPublishHandler(messagePubHandler)
	opts.OnConnect = connectHandler
	opts.OnConnectionLost = connectLostHandler
	client := mqtt.NewClient(opts)
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	sub(client)
	publish(client)

	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)
	<-sigChan

	client.Unsubscribe(cc_topic_from_temp)
	client.Disconnect(250)
}

func publish(client mqtt.Client) {
	num := 10
	for i := 0; i < num; i++ {
		text := fmt.Sprintf("Message %d", i)
		token := client.Publish("/sensor/from/temp/AAAA", 0, false, text)
		token.Wait()
		time.Sleep(time.Second)
	}
}

func sub(client mqtt.Client) {
	token := client.Subscribe(cc_topic_from_temp, 1, nil)
	token.Wait()
	fmt.Printf("Subscribed to topic: %s\n", cc_topic_from_temp)
}
