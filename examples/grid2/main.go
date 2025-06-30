package main

import (
	"fmt"
	"path/filepath"

	"github.com/gradgrind/gominion"
	"examples/mugui"
)

func main() {
	fp, err := filepath.Abs(
		filepath.Join("..", "..", "examples", "grid2.minion"))
	if err != nil {
		panic(err)
	}
	fpm := fmt.Sprintf(
		`[[MINION_FILE,"%s"],[WIDGET,MainWindow,[SHOW]],[RUN]]`,
		fp)
	mugui.MinionGui(fpm, callback)
}

func callback(data string) string {
	fmt.Printf("Go callback got '%s'\n", data)
	v := gominion.ReadMinion(data)
	if e, ok := v.(gominion.MError); ok {
		fmt.Println(" *** Error ***")
		fmt.Println(e)
	} else {
		fmt.Println("  -->")
		fmt.Println(gominion.DumpMinion(v, -1))
	}

	mm := v.(gominion.MList)
	var wname string
	mm.GetString(0, &wname)

	cbr := fmt.Sprintf(`[[WIDGET,"%s",[TEXT,"%s"]]]`, wname, wname)
	fmt.Println("CB: " + cbr)
	return cbr
}
