function askGPT(question, context = "") {
  const apiKey = "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";
  const apiUrl = "https://api.openai.com/v1/chat/completions";

  const systemContext = `Context: small american rural city. 1983 year.`;

  const conversation = [
    { role: "system", content: systemContext + " " + context },
    { role: "user", content: question }
  ];

  return fetch(apiUrl, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      Authorization: `Bearer ${apiKey}`
    },
    body: JSON.stringify({
      // model: "gpt-3.5-turbo",
      model: "gpt-4",
      messages: conversation
    })
  })
    .then(response => response.json())
    .then(data => {
      const reply = data.choices[0].message.content;
      console.log("Assistant:", reply);
      return reply;
    })
    .catch(error => {
      console.error("Error:", error);
      return null;
    });
}
export { askGPT };
