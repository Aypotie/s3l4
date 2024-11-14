package main

import (
	"math/rand"
	"sync"
)

const countSymbols = 500
const gorsCount = 4

type RandSymbols struct {
	mu   *sync.Mutex
	data []rune
	len  int
}

func generator(arr *RandSymbols) {
	randomSymbol := rand.Intn(128)
	arr.data[arr.len] = rune(randomSymbol)
	arr.len++
}

func Run() {
	randomSymbols := RandSymbols{
		data: make([]rune, countSymbols),
	}
	generator(&randomSymbols)
}

func main() {
	Run()
}
