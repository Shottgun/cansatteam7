package backend
import (
	"log"
	"os"
)

func writeToCSV(message string) {
	newFileC, err := os.OpenFile("flightdata.csv", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)

	if err != nil {
		log.Fatal(err)
	}
	defer newFileC.Close()

	_, err2 := newFileC.WriteString(message)

	if err2 != nil {
		log.Fatal(err2)
	}

	newFileC.Close()
}
