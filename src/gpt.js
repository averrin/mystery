function personDescription(person) {
  return {
      goal: "brief literature person description",
      instructions: [
        "tags are private traits, but can be rumored",
        "use max 60% properties",
        "dont show personality.general",
        "show rumors if reputation < 1 or > 5 with 75% prob",
        "max 2 sentences for appearance",
        "dont speak directly about wealth nd reputation, but use it in context",
      ],
      person,
    };
}

function askGPT(question, context = "") {
  const apiKey = "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";
  const apiUrl = "https://api.openai.com/v1/chat/completions";

  const systemContext = `Context: small american rural city. 1983 year. Inspired by Midsomer Murders.`;

  const conversation = [
    { role: "system", content: systemContext + " " + context },
    { role: "user", content: JSON.stringify(question) }
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
      return error;
    });
}
export { askGPT, personDescription };
