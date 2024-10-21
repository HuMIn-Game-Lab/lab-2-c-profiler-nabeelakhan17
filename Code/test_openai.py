import os
import logging
from dotenv import load_dotenv
from huggingface_hub import InferenceClient

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler("test_app.log"),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# Load environment variables
load_dotenv()
hf_api_token = os.getenv("HUGGINGFACE_API_TOKEN")

if not hf_api_token:
    logger.error("Hugging Face API token not found. Please set the HUGGINGFACE_API_TOKEN environment variable.")
    raise ValueError("Hugging Face API token not found. Please set the HUGGINGFACE_API_TOKEN environment variable.")

# Initialize the Inference Client
inference = InferenceClient(token=hf_api_token)

# Define the conversation
system_prompt = "You are a helpful assistant."
user_input = "Hello, how are you?"
conversation = f"{system_prompt}\nUser: {user_input}\nAssistant:"

try:
    # Make the API call using text_generation
    response = inference.text_generation(
        prompt=conversation,
        model="gpt2",
        max_new_tokens=50,
        temperature=0.5,
        do_sample=True,
        return_full_text=False  # Only return the generated text
    )

    # Extract and log the assistant's reply
    assistant_reply = response.strip()
    logger.info(f"Insights: {assistant_reply}")
except Exception as e:
    logger.error(f"Unexpected error: {e}")
