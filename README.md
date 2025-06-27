<a href='https://github.com/nkapila6/mcp-openvto/'><img src='https://github.com/user-attachments/assets/23d6b7fa-5037-4090-990a-b289140bece5' width='200' height='200'></a>

# mcp-openvto
MCP SSE server that allows you to do Virtual Try-On through LLM Chat using a back-end db hosted locally or through the cloud (Couchbase) and Replicate APIs.

## Attribution
The MCP server uses the following services / libraries.
- [cpp-mcp](https://github.com/hkr04/cpp-mcp/): C++ MCP Client and Server library.
- [Eigen C++ library](https://eigen.tuxfamily.org/index.php?title=Main_Page): C++ mathematical linear algebra library.
- [Ollama](https://github.com/jmont-dev/ollama-hpp): C++ bindings for Ollama API.
- [Couchbase] (optional)

## Requirements
- The .CSV file in the `db` folder (required if not using Couchbase)
- CSV files alongside embeddings uploaded to Couchbase (local) or Couchbase Capella (Cloud). Works on free tier!
- Ollama
- ... coming soon...

## Configuration
Coming soon

## Why C++?
I already built 2 MCP servers in Python before and the dependencies and Docker image sizes were too huge. Yes, I could have used an easier example like GoLang but I thought why not!

This project taught me how much I take Python abstractions for granted. 💀 C/C++ humbles you that way.

And yes, doing it in C++ was advantageous.
<img width="263" alt="image" src="https://github.com/user-attachments/assets/9e70c769-b096-4a4c-88e6-0d66e9423730" />

## Example inferences

### Basic example
A basic example using Couchbase (Cloud) services.

https://github.com/user-attachments/assets/30166f61-3b60-4890-b3c9-7ccf5c1c3f32

### Regressive inference (Experimental)
An example of a regressive inference using the MCP server with Couchbase (Cloud) services.
Regressive here means you can use previous output as the base image to virtual transfer on, e.g. T-Shirt Try On -> Pant Try On using T-Shirt Try On Image.

https://github.com/user-attachments/assets/cc85c216-99ec-4aff-a583-48b7735dbcaf

### Meme-ing
Because why not? 😄
Using local CSV vector search.

https://github.com/user-attachments/assets/b564d9a3-a65c-4d52-9acf-c96676b85335

## Contributions
Always welcome to contribute.

## License
This project is licensed under the MIT License.

