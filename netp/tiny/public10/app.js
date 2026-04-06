document.addEventListener("DOMContentLoaded", () => {
  const form = document.getElementById("adder-form");
  const preview = document.getElementById("preview");

  if (!form) return;

  form.addEventListener("input", () => {
    const a = form.a.value;
    const b = form.b.value;

    if (a && b) {
      preview.textContent = `Preview: ${a} + ${b} = ${Number(a) + Number(b)}`;
    } else {
      preview.textContent = "";
    }
  });

  form.addEventListener("submit", (e) => {
    const a = form.a.value;
    const b = form.b.value;

    if (isNaN(a) || isNaN(b)) {
      e.preventDefault();
      alert("Please enter valid numbers");
    }
  });
});
