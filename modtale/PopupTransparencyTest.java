/*
 * Copyright (c) 2026, Modtale. All rights reserved.
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;
import javafx.application.ConditionalFeature;
import javafx.application.Platform;
import javafx.geometry.Side;
import javafx.scene.Scene;
import javafx.scene.SnapshotParameters;
import javafx.scene.paint.Color;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.VBox;
import javafx.stage.PopupWindow;
import javafx.stage.Stage;
import javafx.stage.Window;

/** Native Wayland regression: popup margins must retain alpha, not clear to white. */
public final class PopupTransparencyTest {
    public static void main(String[] args) throws Exception {
        CountDownLatch done = new CountDownLatch(1);
        AtomicReference<Throwable> failure = new AtomicReference<>();
        Platform.startup(() -> {
            try { testPopups(); }
            catch (Throwable error) { failure.set(error); }
            finally { done.countDown(); }
        });
        if (!done.await(30, TimeUnit.SECONDS)) throw new AssertionError("Popup test timed out");
        Platform.exit();
        if (failure.get() != null) throw new AssertionError("Popup transparency regression", failure.get());
        System.out.println("PASS: Wayland transparency, context menus, combo boxes, and tooltips");
    }

    private static void testPopups() {
        require(Platform.isSupported(ConditionalFeature.TRANSPARENT_WINDOW),
                "Wayland alpha surfaces must not depend on the XComposite extension");
        Stage owner = new Stage();
        ContextMenu menu = new ContextMenu(new MenuItem("Pre-release"), new MenuItem("Latest release"));
        ComboBox<String> combo = new ComboBox<>();
        Tooltip tooltip = new Tooltip("Tooltip");
        try {
            Button button = new Button("Dropdown");
            combo.getItems().addAll("Release", "Pre-release");
            owner.setScene(new Scene(new VBox(16, button, combo), 500, 350));
            owner.show();
            menu.show(button, Side.BOTTOM, 0, 8);
            checkAlpha(menu);
            menu.hide();
            combo.show();
            PopupWindow comboPopup = (PopupWindow) Window.getWindows().stream()
                    .filter(window -> window instanceof PopupWindow && window.isShowing())
                    .filter(window -> ((PopupWindow) window).getOwnerWindow() == owner)
                    .findFirst().orElseThrow();
            checkAlpha(comboPopup);
            combo.getSelectionModel().select("Pre-release");
            require("Pre-release".equals(combo.getValue()), "Combo selection failed");
            combo.hide();
            tooltip.show(button, owner.getX() + 100, owner.getY() + 100);
            checkAlpha(tooltip);
        } finally { tooltip.hide(); combo.hide(); menu.hide(); owner.close(); }
    }

    private static void checkAlpha(PopupWindow popup) {
        popup.getScene().getRoot().applyCss();
        popup.getScene().getRoot().layout();
        // Scene.snapshot defaults a null scene fill to white, independently of the
        // native surface. Snapshot the styled content with explicit alpha instead.
        SnapshotParameters parameters = new SnapshotParameters();
        parameters.setFill(Color.TRANSPARENT);
        var image = popup.getScene().getRoot().snapshot(parameters, null);
        var pixels = image.getPixelReader();
        // Sample all four corners: rounded corners / shadow margins must not be opaque white.
        for (int x : new int[]{0, (int) image.getWidth() - 1}) {
            for (int y : new int[]{0, (int) image.getHeight() - 1}) {
                var color = pixels.getColor(x, y);
                require(color.getOpacity() < 0.1, "Opaque popup corner: " + color + " in " + popup.getClass().getSimpleName());
            }
        }
    }

    private static void require(boolean condition, String message) {
        if (!condition) throw new AssertionError(message);
    }
}
