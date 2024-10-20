# Curl - A C-based HTTP Tool

A lightweight tool built in **C** for replicating CURL functionalities, supporting HTTP methods like **GET**, **POST**, **PUT**, and **DELETE**.

## Features
- Supports all basic HTTP methods: **GET**, **POST**, **PUT**, and **DELETE**.
- Enables user-defined payloads for POST/PUT requests, similar to **CURL’s -d** option.
- Lightweight and suitable for learning **socket programming** concepts.

## Technologies Used
- **Language**: C
- **Concepts**: Socket Programming, HTTP Methods

## Getting Started
    gcc curl.h
    gcc Curl.c curlFunctions.c -o C_curl
    ./C_curl options URL
