(function () {
	if (window.__MIAIS_SHARED_HELP_JS_LOCKED__) {
		return;
	}
	window.__MIAIS_SHARED_HELP_JS_LOCKED__ = true;

	function text(value) {
		return value === undefined || value === null ? "" : String(value);
	}

	function emitMessage(prefixValue, typeValue, targetValue) {
		if (!typeValue) {
			return;
		}

		console.log(prefixValue + "_LINK:" + typeValue + ":" + (targetValue || ""));
	}

	function isEditableTarget(targetValue) {
		if (!targetValue || !(targetValue instanceof Element)) {
			return false;
		}

		return !!targetValue.closest("input, textarea, [contenteditable='true']");
	}

	function blockEvent(eventValue) {
		eventValue.preventDefault();
		eventValue.stopPropagation();
	}

	function lockHistoryState() {
		try {
			const currentUrl = window.location.href;
			const lockState = { mounteaHistoryLock: true };
			window.history.replaceState(lockState, "", currentUrl);
			window.history.pushState(lockState, "", currentUrl);
		} catch (errorValue) {
			console.log("MOUNTEA_HISTORY_LOCK_ERROR:" + String(errorValue));
		}
	}

	document.addEventListener(
		"click",
		function (eventValue) {
			const miaisLink = eventValue.target.closest("a[data-miais-type]");
			if (!miaisLink) {
				return;
			}

			eventValue.preventDefault();
			emitMessage("MIAIS", miaisLink.getAttribute("data-miais-type"), miaisLink.getAttribute("data-miais-target"));
		},
		false
	);

	document.addEventListener(
		"contextmenu",
		function (eventValue) {
			blockEvent(eventValue);
		},
		{ capture: true, passive: false }
	);

	["mousedown", "pointerdown", "auxclick", "mouseup"].forEach(function (eventNameValue) {
		document.addEventListener(
			eventNameValue,
			function (eventValue) {
				if (eventValue.button === 2 || eventValue.button === 3 || eventValue.button === 4) {
					blockEvent(eventValue);
				}
			},
			{ capture: true, passive: false }
		);
	});

	document.addEventListener(
		"keydown",
		function (eventValue) {
			const isAltArrowBack = eventValue.altKey && (eventValue.key === "ArrowLeft" || eventValue.key === "ArrowRight");
			const isBrowserBackForwardKey = eventValue.key === "BrowserBack" || eventValue.key === "BrowserForward";
			const isBackspaceNavigation = eventValue.key === "Backspace" && !isEditableTarget(eventValue.target);
			if (isAltArrowBack || isBrowserBackForwardKey || isBackspaceNavigation) {
				blockEvent(eventValue);
			}
		},
		{ capture: true, passive: false }
	);

	document.querySelectorAll("[data-toggle]").forEach(function (toggleValue) {
		toggleValue.addEventListener("click", function () {
			const endpoint = toggleValue.closest("[data-endpoint]");
			const body = endpoint ? endpoint.querySelector(".endpoint-body") : null;
			if (!body) {
				return;
			}

			const isOpen = body.classList.contains("open");
			body.classList.toggle("open", !isOpen);
			toggleValue.textContent = isOpen ? "Show details" : "Hide details";
		});
	});

	window.addEventListener("popstate", lockHistoryState);
	window.addEventListener("hashchange", lockHistoryState);

	lockHistoryState();
})();
